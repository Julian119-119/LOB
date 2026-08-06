# 這個分支要做的事：紅黑樹底層重構

拆掉 `BinarySearchTree`/`RedBlackTree` 的繼承分裂、把節點所有權從 `shared_ptr` 換成 `unique_ptr`（children）+ 裸指標（parent），並手刻一個 STL 相容的 bidirectional iterator，取代目前手動串接 `get_leftmost_node()`/`get_successor()` 的走訪方式。

完整設計細節（node 佈局、rotate/transplant/remove 的所有權轉移序列、iterator 的 `--end()` 處理、風險提醒）見 [`doc/refactor_plan.md`](doc/refactor_plan.md)。這份文件只列這個分支範圍內要做的事，方便隨時對照進度。

## 目標

- [ ] `BinarySearchTree` 退役，`RedBlackTree` 合併成單一扁平類別，節點不再有 base/derived 分裂
- [ ] 節點 `left`/`right` 改 `unique_ptr`，`parent` 改裸指標；不再需要手動 `clear()` 斷循環
- [ ] 手刻 `iterator`/`const_iterator`（bidirectional，`begin()`/`end()`/`operator++`/`operator--`），取代 `get_leftmost_node()`/`get_successor()`
- [ ] `insert`/`insert_emplace`/`remove` 對外簽名不變，`LOB_type.hpp` 不用改

## 不做的事（記錄在 `doc/refactor_plan.md` 附錄，之後再排）

- `Trade` 事件/成交回報機制
- `LOB` 的 copy/move 語意處理
- 接實盤資料相關（feed adapter、執行緒安全）
- sentinel value 改 `std::optional`、price 整數化

## 建議的實作順序

1. **Node + insert 路徑**：`Node` struct、`rotate_left`/`rotate_right`、`owning_slot`、`insert`/`insert_fixup`。先用 `test_BST_RBT.cpp` 驗證。
2. **remove 路徑**：`transplant`、兩子節點 splice（整個重構風險最高的地方）、`remove_fixup`。務必跑 sanitizer。
3. **Iterator**：`successor`/`predecessor`、`rbt_iterator`、`begin()`/`end()`（含 `rightmost_` cache 讓 `--end()` 是 O(1)）。
4. **遷移 `src/LOB_type.cpp`**：6 處呼叫點換成 iterator API（對照表見 `doc/refactor_plan.md` §2.4）。
5. **遷移 `test/test_BST_RBT.cpp`** + 新增 `testRedBlackTreeIterator()`。

## 會動到的檔案

- `include/RedBlackTree.hpp`（原地改寫）
- `include/BinarySearchTree.hpp`（刪除）
- `src/LOB_type.cpp`
- `test/test_BST_RBT.cpp`、`test/test_helper.hpp`、`test/test_main.cpp`
- 不用動：`include/LOB_type.hpp`、`test/test_LOB_order.cpp`、`test/test_LOB_query.cpp`、`benchmark/benchmark.cpp`、`Makefile`

## 驗收checklist

- [ ] `make clean && make test` 全過
- [ ] `make run_benchmark` 跟重構前的基準比較，吞吐量持平或更好
- [ ] `make test CPP_FLAGS="-Iinclude -std=c++17 -Wall -Wextra -g -fsanitize=address,undefined"` 無錯誤（remove 的兩子節點 splice 是重點檢查對象）
- [ ] `grep -rn "get_successor\|get_leftmost_node\|BinarySearchTree\|TreeNode\|dynamic_pointer_cast" --include=*.hpp --include=*.cpp .` 零筆結果

---

合併回 main 之後這份檔案可以直接刪掉，設計細節留在 `doc/refactor_plan.md` 就好。
