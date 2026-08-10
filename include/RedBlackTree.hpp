#ifndef __REDBLACKTREE_HPP__
#define __REDBLACKTREE_HPP__

// #include "BinarySearchTree.hpp"
#include <functional>
#include <memory>
#include <sstream>

enum class Color : uint8_t { RED, BLACK };

template <typename T, typename Compare = std::less<>>
class RedBlackTree : {
 private:
  struct Node : {
    Color color;
    T data;
    Node* parent = nullptr;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;

    template <typename... Args>  // Args ：製作 type T 的參數
    Node(Args&&... args)
        : data(std::forward<Args>(args)...),
          color(Color::RED),
          parent(nullptr) {}
    Node(Args&&... args, Color c)
        : data(std::forward<Args>(args)...), color(c), parent(nullptr) {}
    Node(Args&&... args, Color c, Node* p)
        : data(std::forward<Args>(args)...), color(c), parent(p) {}
    ~Node() = default;
  };

  /*****************************************************************************
   *  private element                                                          *
   *****************************************************************************/
  std::unique_ptr<Node> root_;
  Node* leftmost_node_;

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
  /* static std::unique_ptr<Node> makeRBTreeNode(std::string& s, Node* p =
   * nullptr); */

 public:
  RedBlackTree<T, Compare>() : leftmost_node_(nullptr){};

  bool empty() const { return root_ == nullptr; } /* tree 是否為空 */

  /*****************************************************************************
   *  插入與刪除                                                               *
   *****************************************************************************/
  
  T* insert(T data); /* 一般插入 */
  // 有相同的 data 回傳舊的 data，否則回傳新的 data 的 address

  // K 為用於與 T 比較的的 type
  // Args 為用來製造出 T 的參數
  template <typename K, typename... Args>
  T* insert_emplace(K data, Args&&... args); /* 異構插入 */
  template <typename K>
  void remove(const K& key); /* 異構刪除 */

  /*****************************************************************************
   *  查詢                                                                     *
   *****************************************************************************/
  template <typename K>
  Node* find_node(const K& data);
  Node* get_leftmost_node() const { return leftmost_node_; }
  Node* get_successor(Node* node) const;  // get the next node in in-order
  std::string inorder();
};

/********************************************************************************/

template <typename T, typename Compare>
void RedBlackTree<T, Compare>::setColor(Node* node, Color newcolor) {
  if (node) node->color = newcolor;
}

template <typename T, typename Compare>
Color RedBlackTree<T, Compare>::getColor(Node* node) {
  if (!node)
    return Color::BLACK;
  else
    return node_rbt->color;
}

template <typename T, typename Compare>
void RedBlackTree<T, Compare>::inorder_helper(Node* subroot,
                                              std::stringstream& str,
                                              bool& isFirst) {
  if (!subroot)
    return;
  else {
    inorder_helper(subroot->left.get(), str, isFirst);
    if (isFirst) {
      isFirst = false;
      str << subroot->data;
    } else
      str << ' ' << subroot->data;
    inorder_helper(subroot->right.get(), str, isFirst);
  }
}

/*
template <typename T, typename Compare>
std::unique_ptr<Node> RedBlackTree<T, Compare>::makeRBTreeNode(std::string& s,
                                                               Node* p) {
  if (s == "#") {
    return nullptr;
  } else if (s[0] == '(') {
    s.pop_back();
    s[0] = ' ';
    return std::make_unique<RBTreeNode<T>>(stoi(s), Color::RED, p);
  } else {
    return std::make_unique<RBTreeNode<T>>(stoi(s), Color::BLACK, p);
  }
}
*/

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
    auto zGrandparent = z->parent->parent;
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
  Node<T>* w;
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
          w = static_cast<RBTreeNode<T>*>(parent->right.get());
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
      w = static_cast<RBTreeNode<T>*>(parent->left.get());
      if (getColor(w) == Color::RED) {
        setColor(w, Color::BLACK);
        setColor(parent, Color::RED);
        rotate_right(parent);
        w = static_cast<RBTreeNode<T>*>(parent->left.get());
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
T* RedBlackTree<T, Compare>::insert(T data) {
  Node* node = new Node(data);
  if (!root_) {
    root_.reset(node);
    setColor(node, Color::BLACK) leftmost_node_ = node;
    return &node->data;
  }

  Compare comp;
  Node* parent = root_.get();
  while (parent) {
    if (comp(node->data, parent->data)) {
      if (parent->left) {
        parent = parent->left.get();
      } else {
        parent->left.reset(node);
        node->parent = parent;
        // 檢查是否是插入在最小的節點的左側
        if (parent == leftmost_node_) {
          leftmost_node_ = node;
        }
        break;
      }
    } else if (comp(parent->data, node->data)) {
      if (parent->right) {
        parent = parent->right.get();
      } else {
        parent->right.reset(node);
        node->parent = parent;
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
  Compare comp;
  if (!root_) {
    T new_value(std::forward<Args>(args)...);
    RBTreeNode<T>* node = new RBTreeNode<T>(new_value, Color::BLACK);
    this->leftmost_node_ = node;
    root_.reset(node);
    return &node->data;
  }

  TreeNode<T>* parent = root_.get();
  while (parent) {
    if (comp(data, parent->data)) {
      if (parent->left) {
        parent = parent->left.get();
      } else {
        T new_value(std::forward<Args>(args)...);
        RBTreeNode<T>* node = new RBTreeNode<T>*(new_value);

        parent->left.reset(node);
        node->parent = parent;

        // 檢查是否是插入在最小的節點的左側
        if (parent == this->leftmost_node_) {
          this->leftmost_node_ = node;
        }
        insert_fixup(node);
        return &node->data;
      }
    } else if (comp(parent->data, data)) {
      if (parent->right) {
        parent = parent->right.get();
      } else {
        T new_value(std::forward<Args>(args)...);
        RBTreeNode<T>* node = new RBTreeNode<T>(new_value);
        parent->right.reset(node);
        node->parent = parent;
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
  RBTreeNode<T>* z =
      static_cast<RBTreeNode<T>*>(BinarySearchTree<T, Compare>::find_node(key));
  if (!z) return;

  // 更新 leftmost_node_
  if (z == this->leftmost_node_) {
    if (this->leftmost_node_->right) {
      this->leftmost_node_ = this->leftmost_node_->right.get();
      while (this->leftmost_node_->left) {
        this->leftmost_node_ = this->leftmost_node_->left.get();
      }
    } else {
      this->leftmost_node_ = this->leftmost_node_->parent;
    }
  }

  // y 是指要定替 z 的節點
  // 此為預設只有 0, 1 個 child 才會用自己做暫時的記號
  RBTreeNode<T>* y = z;
  Color y_original_color = getColor(y);  // 紀錄 y 的顏色
  RBTreeNode<T>* x_parent;
  RBTreeNode<T>* x;
  if (!z->left) /* 只有 right child 或沒有 child：用唯一的 child 頂替 */ {
    x = static_cast<RBTreeNode<T>*>(z->right.get());
    x_parent = static_cast<RBTreeNode<T>*>(z->parent);
    BinarySearchTree<T, Compare>::transplant(z, std::move(z->right));
  } else if (!z->right) /* 只有 left child：用唯一的 child 頂替 */ {
    x = static_cast<RBTreeNode<T>*>(z->left.get());
    x_parent = static_cast<RBTreeNode<T>*>(z->parent);
    BinarySearchTree<T, Compare>::transplant(z, std::move(z->left));
  } else /* 有兩個 child */ {
    // 將 y 改為真正替換 z 的節點
    y = static_cast<RBTreeNode<T>*>(z->right.get());
    while (y->left) {
      y = static_cast<RBTreeNode<T>*>(y->left.get());
    }

    y_original_color = getColor(y);                   // 紀錄此時的顏色
    x = static_cast<RBTreeNode<T>*>(y->right.get());  // x 為 y 的 right child
    if (y->parent == z) /* y 為 z 的 right child */ {
      x_parent = y;
      // 開始用 y 頂替 z，並刪除 z
      setColor(y, getColor(z));      // 更換 y 的顏色
      y->left = std::move(z->left);  // 處理 z 的 left subtree
      if (y->left) y->left->parent = y;
      BinarySearchTree<T, Compare>::transplant(
          z, std::move(z->right));  // 將 y 搬到 z 的位置上，並丟掉 z
    } else {
      x_parent = static_cast<RBTreeNode<T>*>(y->parent);
      // 先將 y 的 right subtree（x） 往上移到 y
      std::unique_ptr<TreeNode<T>> tmp_y =
          BinarySearchTree<T, Compare>::transplant(y, std::move(y->right));

      // 開始用 y 頂替掉 z，並刪除 z
      setColor(tmp_y.get(),
               getColor(z));               // 更換 y 的顏色
      tmp_y->right = std::move(z->right);  // 處理 z 的 right subtree
      if (tmp_y->right) tmp_y->right->parent = y;
      tmp_y->left = std::move(z->left);  // 處理 z 的 left subtree
      if (tmp_y->left) y->left->parent = y;
      BinarySearchTree<T, Compare>::transplant(
          z, std::move(tmp_y));  // 將 y 搬到 z 的位置上，並丟掉 z
    }
  }

  if (y_original_color == Color::BLACK) remove_fixup(x, x_parent);
}

#endif