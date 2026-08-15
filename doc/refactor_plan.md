# 紅黑樹底層重構：拆掉 BST、改用 unique_ptr、手刻 STL 相容 Iterator

## 1. 背景與動機

這個 LOB 專案的紅黑樹是從資料結構課程作業延伸而來，`RedBlackTree` 目前 inherit 自 `BinarySearchTree`，node 用 `shared_ptr` 管理。實際追過原始碼後，確認了兩個核心問題：

- **效能瓶頸**：<br>樹的類別之間沒有任何 vtable 呼叫，`RedBlackTree` 一律用 `BinarySearchTree<T,Compare>::xxx(...)` 明確 qualify 呼叫，完全 static binding。真正的成本在 `RBTreeNode::getColor`/`setColor`（`include/RedBlackTree.hpp:44-59`）。每次都要對 `shared_ptr<TreeNode<T>>` 做 `dynamic_pointer_cast<RBTreeNode<T>>`，這個 RTTI cast 在 `insert_fixup`/`remove_fixup` 幾乎每個步驟都會觸發，是 RBT 的隱藏成本。
- **node 的 ownership model 有結構性風險**：<br>`TreeNode` 的 `parent`/`left`/`right` 全部是 `shared_ptr`，形成 parent <-> child 的 reference cycle。目前是靠 destructor 呼叫的 `clear()` 手動走訪、逐一斷開雙向 pointer 才不會 leak。任何新增的樹操作只要忘記同時清空雙向 pointer，就會產生難以 debug 的 memory leak。
- **沒有真正的 iterator**：<br>`get_volume_at_price`、`get_top_k_info`、`order_matching` 內的 FOK pre-check 都是手動串接 `get_leftmost_node()` + `get_successor()`，這正是想用 iterator 取代的重複 pattern。

目的：拆掉 BST/RBT 的類別分裂、把 `shared_ptr` 換成 ownership 更明確的 `unique_ptr`（children）+ raw pointer（parent），並手刻一個 STL 相容的 bidirectional iterator，取代目前的手動走訪 pattern。

## 2. 這次重構的範圍

**要做的事情：**

1. **Node ownership**：<Br>`left`/`right` 改為 `std::unique_ptr<Node>`，`parent` 改為 raw pointer（non-owning）。樹透過 unique_ptr chain 自動 recursive destruct，不再需要手動使用 `clear()` 斷 cycle。
2. **類別結構**：<br>`BinarySearchTree` 整個退役，`include/BinarySearchTree.hpp` 直接刪除。`RedBlackTree` 改寫成單一類別，node 內部直接存 `Color` 欄位，不再有 base/derived node 分裂，也不再有任何 `dynamic_pointer_cast`。
3. **Iterator**：<Br>完整 STL 風格的 bidirectional iterator（`operator++`/`operator--`、`begin()`/`end()`、標準 iterator traits），可搭配 range-for 與 `<algorithm>` 使用。
4. **範圍邊界**：<Br>僅動樹與 iterator，不動 `Trade`、feed adapter、thread safety 等（列在文末附錄，作為之後的獨立規劃項目）。

**目標檔案異動總覽：**

| 檔案                                                                            | 異動                                                                                                                                                   |
| ----------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------- |
| `include/BinarySearchTree.hpp`                                                | **刪除**（`git rm`）。已確認除了 `RedBlackTree.hpp` 和 `test_BST_RBT.cpp` 外，沒有任何地方 reference `TreeNode`/`BinarySearchTree`/`get_successor`/`get_leftmost_node`。 |
| `include/RedBlackTree.hpp`                                                    | **原地改寫**（保留檔名與類別名 `RedBlackTree<T,Compare>`，讓 `LOB_type.hpp` 完全不用改）。                                                                                 |
| `src/LOB_type.cpp`                                                            | 6 處呼叫點改用 iterator API。`insert_emplace`/`remove` 呼叫點不用動。                                                                                              |
| `include/LOB_type.hpp`                                                        | **不需要改動**——只是把 `RedBlackTree<PriceLevel,...>` 當 member 型別使用，沒有直接碰 node 型別。                                                                           |
| `test/test_BST_RBT.cpp` + `test_helper.hpp` + `test_main.cpp`                 | 改寫。                                                                                                                                                  |
| `test/test_LOB_order.cpp`、`test/test_LOB_query.cpp`、`benchmark/benchmark.cpp` | **不需要改動**。只透過 `LOB` 的 public API 操作，是這次重構的 regression test 安全網。                                                                                      |
| `Makefile`                                                                    | 不需要改動。                                                                                                                                               |

## 3. 設計目標的細節

### 3.1 Node 與 ownership model

```cpp
enum class Color : uint8_t { RED, BLACK };

template <typename T, typename Compare = std::less<>>
class RedBlackTree {
 private:
  struct Node {
    T data;
    Color color;
    Node* parent = nullptr;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;

    template <typename... Args>
    Node(Color c, Node* p, Args&&... args)
        : data(std::forward<Args>(args)...), color(c), parent(p) {}
  };
  std::unique_ptr<Node> root_;
  Node* leftmost_  = nullptr;
  Node* rightmost_ = nullptr;   // 新增：讓 --end() 能 O(1) 命中最大值
  ...
};
```

1. 將 Node 寫入 RedBlackTree 的 class 中。
2. 修改 Node 的 constructor，使其可以直接接收額外的參數來建造 type `T`。<Br>這樣就可以順便修掉現有的 `insert_emplace` 中額外複製暫時物件的問題。也代表 Tree 本身不再要求 type `T` 需要可以被複製了。在代有 pmr 的狀況下本來複製就會造成額外的語意問題（共用 pmr）

**Public API**（`insert`/`insert_emplace`/`remove` 的簽名不變）：

```cpp
bool empty() const noexcept;
iterator begin() noexcept; iterator end() noexcept;
const_iterator begin() const noexcept; const_iterator end() const noexcept;
template <typename K> iterator find(const K& key);
template <typename K> const_iterator find(const K& key) const;

T* insert(T data);
template <typename K, typename... Args> T* insert_emplace(K key, Args&&... args);
template <typename K> void remove(const K& key);

std::string inorder();   // 保留作為 debug helper，內部改用 raw Node* 實作
```

`get_leftmost_node()`/`get_successor()` 從 public API 移除，改由 `begin()`/`end()`/`operator++`/`operator--` 取代。

**Copy 問題**：<br>因為 RedBlackTree 改為使用 `distinct_ptr`，因此無法 copy 整顆 RedBlackTree。避免使用 `shared_ptr` 時可以 copy，但會導致兩個 Tree 指向同一堆物件的問題。

### 3.2 核心操作：`owning_slot` / `transplant` / `rotate_left`

```cpp
std::unique_ptr<Node>& owning_slot(Node* n) {
  if (!n->parent) return root_;
  return (n->parent->left.get() == n) ? n->parent->left : n->parent->right;
}

// 注意：transplant 不會動 u 的 left/right，呼叫端必須先把要保留的 subtree
// std::move 出去，否則回傳的 unique_ptr destruct 時會把整棵 subtree 一起炸掉。
std::unique_ptr<Node> transplant(Node* u, std::unique_ptr<Node> v) {
  Node* u_parent = u->parent;
  if (v) v->parent = u_parent;
  std::unique_ptr<Node>& slot = owning_slot(u);
  std::unique_ptr<Node> old = std::move(slot);
  slot = std::move(v);
  return old;
}

void rotate_left(Node* x) {
  if (!x || !x->right) return;
  Node* y = x->right.get();               // Node 位址在 move 過程中不會變，raw pointer 全程有效
  Node* x_parent = x->parent;
  std::unique_ptr<Node>& x_slot = owning_slot(x);

  std::unique_ptr<Node> x_owned = std::move(x_slot);
  std::unique_ptr<Node> y_owned = std::move(x_owned->right);

  x_owned->right = std::move(y_owned->left);
  if (x_owned->right) x_owned->right->parent = x;

  y_owned->left = std::move(x_owned);
  y_owned->left->parent = y;

  y_owned->parent = x_parent;
  x_slot = std::move(y_owned);
}
```

`owning_slot(n)` ：<Br>回傳目前持有 `n` ownership 的那個 `unique_ptr<Node>`（可能是 `root_`，或 `n->parent->left`，或 `n->parent->right`）的 reference。為了後續的 `std::move()` 來準備，先拿取真實的 `unique_ptr<Node>` 才有辦法移動 unique pointer 的指向

### 3.3 `remove()`：兩個 child 的 splice case（整個重構風險最高的部分）

因為在 `Node` 之中，將 pointer 的種類改為了往上指的 `raw pointer` 與兩個向下的 `unique_ptr`，因此必須要在每個分支時，先將 `z -> left / z->right` 完整的 `std::move` 出去，再讓擁有 `z` 的 `unique_ptr` 離開作用域。否則使 `child` 的往上指的 `raw pointer` 產生 dangling pointer.

### 3.4 Iterator 設計

```cpp
static Node* successor(Node* x);    // BinarySearchTree.hpp:100-117 的 raw pointer 版本，邏輯不變
static Node* predecessor(Node* x);  // successor 的鏡像，新寫

template <bool IsConst>
class rbt_iterator {
 public:
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = T;
  using difference_type = std::ptrdiff_t;
  using pointer = std::conditional_t<IsConst, const T*, T*>;
  using reference = std::conditional_t<IsConst, const T&, T&>;

  reference operator*() const { return node_->data; }
  pointer operator->() const { return &node_->data; }
  rbt_iterator& operator++() { node_ = successor(node_); return *this; }
  rbt_iterator& operator--() {
    node_ = node_ ? predecessor(node_) : tree_->rightmost_;  // --end() 命中最大值
    return *this;
  }
  friend bool operator==(const rbt_iterator&, const rbt_iterator&);
  operator rbt_iterator<true>() const;  // iterator -> const_iterator 隱式轉換
 private:
  Node* node_; const RedBlackTree* tree_;
};
using iterator = rbt_iterator<false>;
using const_iterator = rbt_iterator<true>;
```

- `begin()` 回傳 `iterator(leftmost_, this)`，O(1)，沿用現有的 `leftmost_` cache。
- `end()` 回傳 `iterator(nullptr, this)`，空樹時 `begin() == end()` 自然成立。
- **`--end()` 要能命中最大值**：新增一個對稱的 `rightmost_`，讓 `--end()` 保持 O(1)，不需要退化成從 root 往下找。
- `--begin()`（walk 到第一個元素之前）視為 undefined behavior，跟 `std::map` 的 iterator 約定一致，不用額外防呆。

### 3.5 保留 heterogeneous comparator 支援

`find`（新增，取代原本沒被用到的 `find_node`）、`insert_emplace`、`remove` 都維持現有的 template 參數（`K` 可以是 `double`，用 `Less_priceLevel`/`Greater_priceLevel` 的 `operator()(double, const PriceLevel&)` overload 去比較，見 `LOB_type.hpp:135-140,154-159`）。這部分是純粹的 ownership model 替換，template 介面不需要改動。

## 4. 遷移細節

### `src/LOB_type.cpp` 呼叫點對照表

| 位置                                       | 現況                                                                            | 改為                                                                                                                                             |
| ---------------------------------------- | ----------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------- |
| `get_best_bid/ask_price/volume`（17-45 行） | `buyer_tree.get_leftmost_node()->data.getprice()`                             | `buyer_tree.begin()->getprice()`                                                                                                               |
| `get_volume_at_price`（73-96 行）           | leftmost + 逐一 `get_successor` 比對 price（**目前實際是 O(k)，並非 README 宣稱的 O(log N)**） | `auto it = buyer_tree.find(tar_price); return it != buyer_tree.end() ? it->get_total_volume() : VOLUME_NO_VALUE;`（真正的 O(log N) 查找，順便修掉這個複雜度落差） |
| `get_top_k_info`（98-117 行）               | leftmost + `get_successor` 迴圈                                                 | `for (auto it = tree.begin(); it != tree.end() && i < k; ++it, ++i) info.emplace_back(it->getprice(), it->get_total_volume());`                |
| `order_matching` FOK 預檢查（122-150 行）      | `while(1)` + `get_successor`                                                  | 相同迴圈結構，`order_it = X_tree.get_successor(order_it)` 換成 `++order_it`，null 檢查換成 `order_it != X_tree.end()`                                        |
| `order_matching`（155, 193 行）             | `&seller_tree.get_leftmost_node()->data`                                      | `&*seller_tree.begin()`                                                                                                                        |

`insert_emplace`/`remove` 呼叫點維持原樣，因為這兩個 method 的簽名沒有變。

### `test/test_BST_RBT.cpp` 改動

- `testSmallest()`（9-63 行）：`BinarySearchTree<int>` 改成 `RedBlackTree<int>`，`tree.get_leftmost_node()->data` 改成 `*tree.begin()`。
- `testInt()`/`testString()`（65-102 行）：只需要把型別從 `BinarySearchTree<T>` 換成 `RedBlackTree<T>`，`.inorder()` 呼叫本身不用改（`inorder()` 保留在合併後的類別上）。
- `testRedBlackTrees()`（104-146 行）：手動 successor 走訪（127-132 行）改成 `auto it = tree.begin(); ...; ++it;`。
- **新增** `testRedBlackTreeIterator()`（在 `test_helper.hpp` 宣告、`test_BST_RBT.cpp` 實作、`test_main.cpp` 呼叫）：涵蓋空樹 `begin()==end()`、`--end()` 命中最大值、正向/反向完整走訪、以及 `std::is_sorted(tree.begin(), tree.end())` 這類 `<algorithm>` 相容性檢查。獨立成一個新函式，方便 review 時清楚看到 iterator 專屬的測試覆蓋。

## 5. 風險

- **`owning_slot()` 回傳的 reference 存續時間**：<Br>因為 `root_`/`parent->left`/`parent->right` 是固定位址的 member（不會因為這次操作而被重新配置），這個 reference 在整個 rotation/transplant 呼叫期間都有效。但這是被寫入的方式（在 `Node` 中使用了 `unique_ptr`）所保證的，因此在 `owning_slot` 上寫一行註解說明，避免未來有人把 children 換成 `vector<unique_ptr<Node>>` 之類的 container 型別而不小心破壞這個前提。
- **`transplant` 的「不動 children」contract**必須用註解明確標出，呼叫端要在呼叫前把想保留的 subtree `std::move` 出去。
- **`remove()` 兩個 child 的分支是整個重構風險最高的地方**：務必確保 `z->left`/`z->right` 在 `old_z` 離開作用域前已經完全被 move 走，這是避免 double-free 的關鍵。
- Fixup 迴圈裡在呼叫過 `rotate_left`/`rotate_right` 之後又讀取 `->parent`，這點跟現有 shared_ptr 版本行為一致（rotation 本身就會更新受影響 node 的 `parent`），不是新風險，但值得在 code review 時明確提醒不要「優化」成快取一個舊的 `Node*`。

## 6. 實作順序與驗證

1. **Baseline**：在動手前先跑一次 `make test` 和 `make run_benchmark`，記錄目前的 benchmark 數字作為「重構前」基準。
2. 在 feature branch 上進行（Makefile 把所有測試檔案連結成同一個 `test_program`，樹和 LOB 是耦合的，中間過程可能編不過，只要求 branch tip 過關即可）。
3. 改寫 `include/RedBlackTree.hpp`，刪除 `include/BinarySearchTree.hpp`。
4. 改寫 `test/test_BST_RBT.cpp` + `test_helper.hpp`/`test_main.cpp`。可以先單獨編譯這個檔案（`g++ -std=c++17 -Iinclude -fsyntax-only test/test_BST_RBT.cpp`）驗證新樹的 API，不用等 `LOB_type.cpp` 一起編。
5. 改 `src/LOB_type.cpp` 六處呼叫點。
6. `make clean && make test`——第一個全系統驗證關卡，所有 `assert` 型測試（含未改動的 `test_LOB_order.cpp`/`test_LOB_query.cpp`）都要跑過。
7. `make run_benchmark`，跟步驟 1 的基準比較——拿掉 `dynamic_pointer_cast` 和 `shared_ptr` 的 atomic refcount，理論上效能應該持平或更好。
8. **建議額外跑一次 sanitizer**：`make test CPP_FLAGS="-Iinclude -std=c++17 -Wall -Wextra -g -fsanitize=address,undefined"`，這是專門針對 unique_ptr 改寫（尤其是 `remove()` 的兩個 child splice）最有效的驗證方式，能直接抓到 double-free/use-after-free。`benchmark.cpp` 現成的 10M 次隨機下單/取消操作在 sanitizer 下也是很好的隨機覆蓋（數量可視情況調小，因為 ASan 下會慢很多）。
9. 最後跑一次 `grep -rn "get_successor\|get_leftmost_node\|BinarySearchTree\|TreeNode\|dynamic_pointer_cast" --include=*.hpp --include=*.cpp .` 確認遷移完全乾淨，應該零筆結果。

**驗收標準彙總：**

- [ ] `make clean && make test` 全部通過（process exit code 0，無 assert 失敗）
- [ ] `make run_benchmark` 跟重構前基準比較，吞吐量持平或提升
- [ ] `make test CPP_FLAGS="-Iinclude -std=c++17 -Wall -Wextra -g -fsanitize=address,undefined"` 無任何記憶體錯誤
- [ ] grep 掃描確認沒有殘留的 `get_successor`/`get_leftmost_node`/`BinarySearchTree`/`TreeNode`/`dynamic_pointer_cast`

**這次重構的關鍵檔案：**

- `include/RedBlackTree.hpp`
- `include/BinarySearchTree.hpp`（刪除）
- `src/LOB_type.cpp`
- `test/test_BST_RBT.cpp`
- `test/test_helper.hpp`
- `test/test_main.cpp`

## 附錄：其他建議（不在本次重構範圍內，供未來規劃參考）

這次背景調查時另外整理了一份技術債清單，跟這次的樹重構無直接關係，但涉及「還有哪些問題」與「未來想接實盤資料」，一併記錄供之後排優先序：

- **`place_order`/`cancel_order` 目前沒有任何成交回報機制**（`src/LOB_type.cpp:119-230` 回傳 `void`，撮合只是直接改 `Order.volume`，沒有 `Trade` event）。這是接實盤前一定要補的基礎設施（測試、日誌、視覺化都需要），但需要另外設計 `Trade` struct 與回傳型別，不建議跟這次的樹重構混在一起做。
- **`LOB` 沒有明確處理 copy/move 語意**：`pool`（`unsynchronized_pool_resource`）被 `order_map` 和每個 `PriceLevel::order_queue` 綁定，若 `LOB` 被 copy 會產生指向錯誤 pool 的 dangling container，目前沒有測試覆蓋到，建議之後 `=delete` 或正確實作。
- **完全沒有網路/檔案 I/O/JSON/CSV 解析**、**完全 single-threaded**（`pmr::unsynchronized_pool_resource` 明確非 thread-safe）——這兩點是接實盤資料前必須從零設計的部分：需要一個 feed adapter 把外部格式轉成 `Order`/操作事件，以及決定 ingestion thread 跟撮合邏輯之間的同步模型（single-writer + queue 是常見做法）。
- **架構上的提醒**：真實交易所的 L2/L3 市場資料通常是「已經撮合完的」book delta（add/cancel/execute），接實盤資料時常常不是「餵一筆新單進自己的撮合引擎」，而是「套用交易所回報的 book 變化」去維護 book 狀態。這跟目前 LOB 自己做撮合模擬是兩種不同的使用情境，建議在真正規劃接實盤資料時，先確認是要做「shadow 撮合模擬」還是「純粹 book 複製」，這會大幅影響 `LOB` 的介面設計。
- 較小的技術債：`insert_emplace`/`BST::insert` 邏輯重複（這次重構會順便解決，因為 BST 整個退役）、sentinel value（`PRICE_NO_VALUE` 等）可考慮改成 `std::optional`、`price` 用 `double` 存在實盤情境下可能有精度問題（可考慮改成整數化的 tick 表示）——這幾項改動面較大，建議等這次重構穩定、其他技術債處理完後再評估。
