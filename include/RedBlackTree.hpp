#ifndef __REDBLACKTREE_HPP__
#define __REDBLACKTREE_HPP__

#include <cstddef>
#include <functional>
#include <iterator>
#include <memory>
#include <queue>
#include <random>
#include <sstream>
#include <type_traits>

enum class Color : uint8_t { RED, BLACK };

template <typename T, typename Compare = std::less<>>
class RedBlackTree {
 private:
  struct Node {
    Color color;
    T data;
    Node* parent = nullptr;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;

    template <typename... Args>  // Args ：製作 type T 的參數
    Node(Args&&... args)
        : color(Color::RED),
          data(std::forward<Args>(args)...),
          parent(nullptr) {}
    template <typename... Args>
    Node(Color c, Args&&... args)
        : color(c), data(std::forward<Args>(args)...), parent(nullptr) {}
    template <typename... Args>
    Node(Color c, Node* p, Args&&... args)
        : color(c), data(std::forward<Args>(args)...), parent(p) {}
    ~Node() = default;
  };

  /*****************************************************************************
   *  private element                                                          *
   *****************************************************************************/
  std::unique_ptr<Node> root_;
  Node* leftmost_node_;
  Node* rightmost_node_;

  /*****************************************************************************
   *  transplant                                                               *
   *****************************************************************************/
  // 用來將 v 頂替 u，但只處理「父子連結」，不處理 left and right subtree
  // 回傳暫存 u 的 unique_ptr
  std::unique_ptr<Node> transplant(Node* u, std::unique_ptr<Node> v);

  // 為了 transplant 使用 std::move() ，指向 n 的 unique_ptr，並回傳
  // reference，使它可以用 std::move()
  std::unique_ptr<Node>& owning_slot(Node* n);

  /*****************************************************************************
   *  rebalance function                                                       *
   *****************************************************************************/
  void rotate_left(Node* x);
  void rotate_right(Node* x);
  void insert_fixup(Node* z);
  void remove_fixup(Node* x, Node* parent);

  /*****************************************************************************
   *  other helper function  *
   *****************************************************************************/
  static Color getColor(Node* node);
  static void setColor(Node* node, Color newcolor);
  void inorder_helper(Node* subroot, std::stringstream& str, bool& isFirst);
  static Node* successor(Node* node);    // get the next node in in-order sort
  static Node* predecessor(Node* node);  // get the previous node in in-order
  template <typename K>
  Node* find_node_ptr(const K& data);

 public:
  /*****************************************************************************
   *  iterator                                                                 *
   *****************************************************************************/
  template <bool isConst>
  struct rbt_iterator {
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = std::conditional_t<isConst, const T*, T*>;
    using reference = std::conditional_t<isConst, const T&, T&>;

    explicit rbt_iterator(Node* ptr, const RedBlackTree* tree_ptr)
        : node_(ptr), tree_(tree_ptr) {}
    explicit rbt_iterator() : node_(nullptr), tree_(nullptr) {}

    reference operator*() const { return node_->data; }
    pointer operator->() const { return &node_->data; }

    rbt_iterator& operator++() {
      if (node_) node_ = successor(node_);
      return *this;
    }
    rbt_iterator operator++(int) {
      rbt_iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    rbt_iterator& operator--() {
      if (node_ == nullptr)
        node_ = tree_->rightmost_node_;
      else
        node_ = predecessor(node_);
      return *this;
    }
    rbt_iterator operator--(int) {
      rbt_iterator tmp = *this;
      --(*this);
      return tmp;
    }

    friend bool operator==(const rbt_iterator& a, const rbt_iterator& b) {
      return a.tree_ == b.tree_ && a.node_ == b.node_;
    }
    friend bool operator!=(const rbt_iterator& a, const rbt_iterator& b) {
      return a.tree_ != b.tree_ || a.node_ != b.node_;
    }

    /* conversion operator */
    operator rbt_iterator<true>() const {
      return rbt_iterator<true>(node_, tree_);
    }

   private:
    Node* node_;
    const RedBlackTree* tree_;
  };
  using iterator = rbt_iterator<false>;
  using const_iterator = rbt_iterator<true>;

  RedBlackTree() : leftmost_node_(nullptr), rightmost_node_(nullptr) {}

  bool empty() const { return root_ == nullptr; } /* tree 是否為空 */

  /*****************************************************************************
   *  插入與刪除                                                               *
   *****************************************************************************/

  T* insert(T data); /* 一般插入 */
  // 有相同的 data 回傳舊的 data，否則回傳新的 data 的 address

  template <typename K, typename... Args>
  // K 為用於與 T 比較的的 type，Args 為用來製造出 T 的參數
  T* insert_emplace(K data, Args&&... args); /* 異構插入 */
  // 有相同的 data 則回傳就的 data 的 address，否則回傳新的

  template <typename K>
  void remove(const K& key); /* 異構刪除 */

  /*****************************************************************************
   *  查詢                                                                     *
   *****************************************************************************/
  template <typename K>
  iterator find_node(const K& data);
  template <typename K>
  const_iterator find_node(const K& data) const;
  iterator begin() { return iterator(this->leftmost_node_, this); }
  const_iterator begin() const {
    return const_iterator(this->leftmost_node_, this);
  }
  iterator end() { return iterator(nullptr, this); }
  const_iterator end() const { return const_iterator(nullptr, this); }

  /*****************************************************************************
   *  Debug 與 test 用                                                         *
   *****************************************************************************/
  std::string serialization();
  std::string inorder();

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-template-friend"
  friend void test_invariant();
  friend int dfs_test_invariant(Node* node);
  friend void fuzz_test();
#pragma GCC diagnostic pop
};

/********************************************************************************/

template <typename T, typename Compare>
void RedBlackTree<T, Compare>::inorder_helper(Node* subroot,
                                              std::stringstream& str,
                                              bool& isFirst) {
  if (!subroot) return;

  inorder_helper(subroot->left.get(), str, isFirst);
  if (isFirst) {
    isFirst = false;
    str << subroot->data;
  } else {
    str << ' ' << subroot->data;
  }

  if (subroot->color == Color::RED) {
    str << "(R)";
  } else {
    str << "(B)";
  }
  inorder_helper(subroot->right.get(), str, isFirst);
}

template <typename T, typename Compare>
std::string RedBlackTree<T, Compare>::inorder() {
  if (root_) {
    std::stringstream inorderString;
    bool isFirst = true;
    inorder_helper(root_.get(), inorderString, isFirst);
    return inorderString.str();
  } else {
    return "";
  }
}

template <typename T, typename Compare>
void RedBlackTree<T, Compare>::setColor(Node* node, Color newcolor) {
  if (node) node->color = newcolor;
}

template <typename T, typename Compare>
Color RedBlackTree<T, Compare>::getColor(Node* node) {
  if (!node)
    return Color::BLACK;
  else
    return node->color;
}

template <typename T, typename Compare>
void RedBlackTree<T, Compare>::rotate_left(Node* x) {
  if (!x || !x->right) return;

  Node* substituteNode = x->right.get();
  std::unique_ptr<Node> tmp_x = transplant(x, std::move(x->right));
  if (substituteNode->left) {
    tmp_x->right = std::move(substituteNode->left);
    tmp_x->right->parent = tmp_x.get();
    tmp_x->parent = substituteNode;
    substituteNode->left = std::move(tmp_x);
  } else {
    tmp_x->parent = substituteNode;
    substituteNode->left = std::move(tmp_x);
  }
}

template <typename T, typename Compare>
void RedBlackTree<T, Compare>::rotate_right(Node* y) {
  if (!y || !y->left) return;

  Node* substituteNode = y->left.get();
  std::unique_ptr<Node> tmp_y = transplant(y, std::move(y->left));
  if (substituteNode->right) {
    tmp_y->left = std::move(substituteNode->right);
    tmp_y->left->parent = tmp_y.get();
    tmp_y->parent = substituteNode;
    substituteNode->right = std::move(tmp_y);
  } else {
    tmp_y->parent = substituteNode;
    substituteNode->right = std::move(tmp_y);
  }
}

template <typename T, typename Compare>
void RedBlackTree<T, Compare>::insert_fixup(Node* z) {
  if (z == root_.get() || z->parent == root_.get()) {
    if (getColor(root_.get()) == Color::RED) {
      setColor(root_.get(), Color::BLACK);
    }
    return;
  }
  if (getColor(z) == Color::RED && getColor(z->parent) == Color::RED) {
    Node* zGrandparent = z->parent->parent;
    if (zGrandparent->left.get() == z->parent) /* Left */ {
      auto zUncle = zGrandparent->right.get();
      if (getColor(zUncle) == Color::RED) /* uncle is red */ {
        setColor(zUncle, Color::BLACK);
        setColor(z->parent, Color::BLACK);
        setColor(zGrandparent, Color::RED);
        insert_fixup(zGrandparent);
      } else if (zGrandparent->left->right.get() == z) /* case 2: LR */ {
        rotate_left(z->parent);
        insert_fixup(z->left.get());
      } else /* case 3: LL */ {
        setColor(zGrandparent, Color::RED);
        setColor(zGrandparent->left.get(), Color::BLACK);
        rotate_right(zGrandparent);
        insert_fixup(z);
      }
    } else /* Right */ {
      auto zUncle = zGrandparent->left.get();
      if (getColor(zUncle) == Color::RED) /* case 1 */ {
        setColor(z->parent, Color::BLACK);
        setColor(zUncle, Color::BLACK);
        setColor(zGrandparent, Color::RED);
        insert_fixup(zGrandparent);
      } else if /* case 2 */ (zGrandparent->right &&
                              zGrandparent->right->left.get() == z) {
        rotate_right(z->parent);
        insert_fixup(z->right.get());
      } else /* case 3 */ {
        setColor(z->parent, Color::BLACK);
        setColor(zGrandparent, Color::RED);
        rotate_left(zGrandparent);
        insert_fixup(z);
      }
    }
  } else {
    if (getColor(root_.get()) == Color::RED) {
      setColor(root_.get(), Color::BLACK);
    }
    return;
  }
}

template <typename T, typename Compare>
void RedBlackTree<T, Compare>::remove_fixup(Node* x, Node* parent) {
  Node* w;
  while (x != root_.get() && getColor(x) == Color::BLACK) {
    if (x == parent->left.get()) {
      w = parent->right.get();
      if (getColor(w) == Color::RED) {
        setColor(w, Color::BLACK);
        setColor(parent, Color::RED);
        rotate_left(parent);
        w = parent->right.get();
      } else if (getColor(w->left.get()) == Color::BLACK &&
                 getColor(w->right.get()) == Color::BLACK) {
        setColor(w, Color::RED);
        x = parent;
        parent = parent->parent;
      } else {
        if (getColor(w->right.get()) == Color::BLACK &&
            getColor(w->left.get()) == Color::RED) {
          setColor(w->left.get(), Color::BLACK);
          setColor(w, Color::RED);
          rotate_right(w);
          w = parent->right.get();
        }

        setColor(w, getColor(parent));
        setColor(parent, Color::BLACK);
        setColor(w->right.get(), Color::BLACK);
        rotate_left(parent);
        if (getColor(root_.get()) == Color::RED)
          setColor(root_.get(), Color::BLACK);
        x = root_.get();
      }
    } else {
      w = parent->left.get();
      if (getColor(w) == Color::RED) {
        setColor(w, Color::BLACK);
        setColor(parent, Color::RED);
        rotate_right(parent);
        w = parent->left.get();
      } else if (getColor(w->left.get()) == Color::BLACK &&
                 getColor(w->right.get()) == Color::BLACK) {
        setColor(w, Color::RED);
        x = parent;
        parent = x->parent;
      } else {
        if (getColor(w->right.get()) == Color::RED &&
            getColor(w->left.get()) == Color::BLACK) {
          setColor(w->right.get(), Color::BLACK);
          setColor(w, Color::RED);
          rotate_left(w);
          w = parent->left.get();
        }

        setColor(w, getColor(parent));
        setColor(parent, Color::BLACK);
        setColor(w->left.get(), Color::BLACK);
        rotate_right(parent);
        x = root_.get();
      }
    }
  }
  setColor(x, Color::BLACK);
}

template <typename T, typename Compare>
std::unique_ptr<typename RedBlackTree<T, Compare>::Node>&
RedBlackTree<T, Compare>::owning_slot(Node* n) {
  if (!n->parent) {
    return root_;
  } else {
    return (n->parent->left.get() == n) ? n->parent->left : n->parent->right;
  }
}

template <typename T, typename Compare>
std::unique_ptr<typename RedBlackTree<T, Compare>::Node>
RedBlackTree<T, Compare>::transplant(Node* u, std::unique_ptr<Node> v) {
  // 更改向上的路線
  if (v) v->parent = u->parent;
  // 將 v 給儲存 u 的 unique_ptr 與將 u 給暫存的 unique_ptr (old)
  std::unique_ptr<Node>& slot = owning_slot(u);
  std::unique_ptr<Node> old = std::move(slot);
  slot = std::move(v);
  return old;
}

template <typename T, typename Compare>
template <typename K>
/* find_node: herogeneously find */
typename RedBlackTree<T, Compare>::iterator RedBlackTree<T, Compare>::find_node(
    const K& key) {
  // 確保 O(1) 時間內可以找到最小與最大的 node
  Compare comp;
  if (leftmost_node_ &&
      (!comp(leftmost_node_->data, key) && !comp(key, leftmost_node_->data))) {
    return iterator(leftmost_node_, this);
  }
  if (rightmost_node_ && (!comp(rightmost_node_->data, key) &&
                          !comp(key, rightmost_node_->data))) {
    return iterator(rightmost_node_, this);
  }

  Node* node = root_.get();
  while (node) {
    if (comp(key, node->data)) {
      node = node->left.get();
    } else if (comp(node->data, key)) {
      node = node->right.get();
    } else {
      return iterator(node, this);
    }
  }

  return iterator(nullptr, this);
}

template <typename T, typename Compare>
template <typename K>
/* find_node: herogeneously find */
typename RedBlackTree<T, Compare>::const_iterator RedBlackTree<T, Compare>::find_node(
    const K& key) const {
  // 確保 O(1) 時間內可以找到最小與最大的 node
  Compare comp;
  if (leftmost_node_ &&
      (!comp(leftmost_node_->data, key) && !comp(key, leftmost_node_->data))) {
    return const_iterator(leftmost_node_, this);
  }
  if (rightmost_node_ && (!comp(rightmost_node_->data, key) &&
                          !comp(key, rightmost_node_->data))) {
    return const_iterator(rightmost_node_, this);
  }

  Node* node = root_.get();
  while (node) {
    if (comp(key, node->data)) {
      node = node->left.get();
    } else if (comp(node->data, key)) {
      node = node->right.get();
    } else {
      return const_iterator(node, this);
    }
  }

  return const_iterator(nullptr, this);
}

template <typename T, typename Compare>
template <typename K>
/* find_node: herogeneously find */
typename RedBlackTree<T, Compare>::Node*
RedBlackTree<T, Compare>::find_node_ptr(const K& key) {
  // 確保 O(1) 時間內可以找到最小與最大的 node
  Compare comp;
  if (leftmost_node_ &&
      (!comp(leftmost_node_->data, key) && !comp(key, leftmost_node_->data))) {
    return leftmost_node_;
  }
  if (rightmost_node_ && (!comp(rightmost_node_->data, key) &&
                          !comp(key, rightmost_node_->data))) {
    return rightmost_node_;
  }

  Node* node = root_.get();
  while (node) {
    if (comp(key, node->data)) {
      node = node->left.get();
    } else if (comp(node->data, key)) {
      node = node->right.get();
    } else {
      return node;
    }
  }

  return nullptr;
}

template <typename T, typename Compare>
/* 找尋比自己大的節點中，擁有最小值的 node */
typename RedBlackTree<T, Compare>::Node* RedBlackTree<T, Compare>::successor(
    Node* node) {
  if (!node) return nullptr;

  Node* curr = node;
  if (curr->right) {
    curr = curr->right.get();
    while (curr->left) curr = curr->left.get();
  } else {
    Node* p = curr->parent;
    while (p && p->left.get() != curr) {
      curr = p;
      p = p->parent;
    }
    curr = p;
  }

  return curr;
}

template <typename T, typename Compare>
typename RedBlackTree<T, Compare>::Node* RedBlackTree<T, Compare>::predecessor(
    Node* node) {
  Node* curr = node;
  if (node->left) {
    curr = curr->left.get();
    while (curr->right) curr = curr->right.get();
  } else {
    Node* p = curr->parent;
    while (p && p->right.get() != curr) {
      curr = p;
      p = p->parent;
    }
    curr = p;
  }

  return curr;
}

template <typename T, typename Compare>
T* RedBlackTree<T, Compare>::insert(T data) {
  if (!root_) {
    Node* node = new Node(data);
    root_.reset(node);
    setColor(node, Color::BLACK);
    leftmost_node_ = node;
    rightmost_node_ = node;
    return &node->data;
  }

  Compare comp;
  Node* node = nullptr;
  Node* parent = root_.get();
  while (parent) {
    if (comp(data, parent->data)) {
      if (parent->left) {
        parent = parent->left.get();
      } else {
        node = new Node(data);
        parent->left.reset(node);
        node->parent = parent;
        // 檢查是否是插入在最小的節點的左側
        if (parent == leftmost_node_) {
          leftmost_node_ = node;
        }
        break;
      }
    } else if (comp(parent->data, data)) {
      if (parent->right) {
        parent = parent->right.get();
      } else {
        node = new Node(data);
        parent->right.reset(node);
        node->parent = parent;
        //  檢查是否插入在最大節點的右側
        if (parent == rightmost_node_) {
          rightmost_node_ = node;
        }
        break;
      }
    } else {
      return &parent->data;
    }
  }

  insert_fixup(node);
  return &node->data;
}

template <typename T, typename Compare>
template <typename K, typename... Args>
T* RedBlackTree<T, Compare>::insert_emplace(K data, Args&&... args) {
  if (!root_) {
    T new_value(std::forward<Args>(args)...);
    Node* node = new Node(Color::BLACK, new_value);
    leftmost_node_ = node;
    rightmost_node_ = node;
    root_.reset(node);
    return &node->data;
  }

  Compare comp;
  Node* parent = root_.get();
  while (parent) {
    if (comp(data, parent->data)) {
      if (parent->left) {
        parent = parent->left.get();
      } else {
        // 用 T 的 constructure 建造 T
        T new_value(std::forward<Args>(args)...);
        Node* node = new Node(new_value);

        parent->left.reset(node);
        node->parent = parent;

        // 檢查是否是插入在最小的節點的左側
        if (parent == leftmost_node_) {
          leftmost_node_ = node;
        }
        insert_fixup(node);
        return &node->data;
      }
    } else if (comp(parent->data, data)) {
      if (parent->right) {
        parent = parent->right.get();
      } else {
        T new_value(std::forward<Args>(args)...);
        Node* node = new Node(new_value);
        parent->right.reset(node);
        node->parent = parent;

        // 檢查是否是插入在最大節點的右側
        if (parent == rightmost_node_) {
          rightmost_node_ = node;
        }
        insert_fixup(node);
        return &node->data;
      }
    } else {
      return &parent->data;
    }
  }

  return nullptr;
}

template <typename T, typename Compare>
template <typename K>
void RedBlackTree<T, Compare>::remove(const K& key) {
  // z 為真正要刪除的節點
  Node* z = find_node_ptr(key);
  if (!z || !root_) return;

  // 更新 leftmost_node_
  if (z == leftmost_node_) {
    if (leftmost_node_->right) {
      leftmost_node_ = leftmost_node_->right.get();
      while (leftmost_node_->left) {
        leftmost_node_ = leftmost_node_->left.get();
      }
    } else {
      leftmost_node_ = leftmost_node_->parent;
    }
  }

  // 更新 rightmost_node_
  if (z == rightmost_node_) {
    if (rightmost_node_->left) {
      rightmost_node_ = rightmost_node_->left.get();
      while (rightmost_node_->right) {
        rightmost_node_ = rightmost_node_->right.get();
      }
    } else {
      rightmost_node_ = rightmost_node_->parent;
    }
  }

  // y 是指要定替 z 的節點
  // 此為預設只有 0, 1 個 child 才會用自己做暫時的記號
  Node* y = z;
  Color y_original_color = getColor(y);  // 紀錄 y 的顏色
  Node* x_parent;
  Node* x;
  if (!z->left) /* 只有 right child 或沒有 child：用唯一的 child 頂替 */ {
    x = z->right.get();
    x_parent = z->parent;
    transplant(z, std::move(z->right));
  } else if (!z->right) /* 只有 left child：用唯一的 child 頂替 */ {
    x = z->left.get();
    x_parent = z->parent;
    transplant(z, std::move(z->left));
  } else /* 有兩個 child */ {
    // 將 y 改為真正替換 z 的節點
    y = z->right.get();
    while (y->left) {
      y = y->left.get();
    }

    y_original_color = getColor(y);  // 紀錄此時的顏色
    x = y->right.get();              // x 為 y 的 right child
    if (y->parent == z) /* y 為 z 的 right child */ {
      x_parent = y;

      /* 用 y 頂替 z，並刪除 z */
      setColor(y, getColor(z));      // 更換 y 的顏色
      y->left = std::move(z->left);  // 處理 z 的 left subtree
      if (y->left) y->left->parent = y;
      transplant(z, std::move(z->right));  // 將 y 搬到 z 的位置上，並丟掉 z
    } else {
      x_parent = y->parent;
      // 先將 y 的 right subtree（x） 往上移到 y
      std::unique_ptr<Node> tmp_y = transplant(y, std::move(y->right));

      /* 用 y 頂替掉 z，並刪除 z */
      setColor(tmp_y.get(), getColor(z));  // 更換 y 的顏色
      tmp_y->right = std::move(z->right);  // 處理 z 的 right subtree
      if (tmp_y->right) tmp_y->right->parent = y;
      tmp_y->left = std::move(z->left);  // 處理 z 的 left subtree
      if (tmp_y->left) y->left->parent = y;
      transplant(z, std::move(tmp_y));  // 將 y 搬到 z 的位置上，並丟掉 z
    }
  }

  if (y_original_color == Color::BLACK) remove_fixup(x, x_parent);
}

template <typename T, typename Compare>
std::string RedBlackTree<T, Compare>::serialization() {
  if (!root_) return "#";

  std::queue<Node*> q;
  q.push(root_.get());
  int non_null_node = 1;
  int curr_level = 0;
  std::stringstream ss;
  while (non_null_node) {
    int level_size = q.size();
    ss << 'L' << curr_level << ": ";
    while (level_size--) {
      Node* node = q.front();
      q.pop();
      if (node) {
        non_null_node--;
        ss << node->data;
        if (node->color == Color::RED) {
          ss << "(R) ";
        } else {
          ss << "(B) ";
        }
        q.push(node->left.get());
        q.push(node->right.get());
        if (node->left) non_null_node++;
        if (node->right) non_null_node++;
      } else {
        ss << "#    ";
        q.push(nullptr);  // 為了保持印出來的圖是完整的，必須遇到 null 時
        q.push(nullptr);  // 將他的 LC, RC 的 nullptr push 進去
      }
    }
    ss << '\n';
    curr_level++;
  }

  return ss.str();
}

#endif