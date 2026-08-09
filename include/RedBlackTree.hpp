#ifndef __REDBLACKTREE_HPP__
#define __REDBLACKTREE_HPP__

#include "BinarySearchTree.hpp"

enum class Color { RED, BLACK };

template <typename T>
struct RBTreeNode : TreeNode<T> {
  Color color;
  RBTreeNode<T>(T v) : TreeNode<T>(v), color(Color::RED) {}
  RBTreeNode<T>(T v, Color c) : TreeNode<T>(v), color(c) {}
  RBTreeNode<T>(T v, Color c, TreeNode<T>* p) : TreeNode<T>(v, p), color(c) {}
  ~RBTreeNode<T>() = default;

  static std::unique_ptr<RBTreeNode<T>> makeRBTreeNode(
      std::string& s, TreeNode<T>* p = nullptr);
  static Color getColor(TreeNode<T>* node);
  static void setColor(TreeNode<T>* node, Color newcolor);
};

template <typename T, typename Compare = std::less<>>
class RedBlackTree : public BinarySearchTree<T, Compare> {
 public:
  RedBlackTree<T, Compare>() : BinarySearchTree<T, Compare>(){};

  T* insert(T data);  // 如果有相同的 node 則回傳舊的 node，否則傳新的節點

  /* K 為用於與 T 比較的的 type */
  /* Args 為用來製造出 T 的參數 */
  template <typename K, typename... Args>
  T* insert_emplace(K data, Args&&... args);  // 異構插入
  template <typename K>
  void remove(const K& key);  // 異構刪除

 private:
  void insert_fixup(RBTreeNode<T>* z);  // rebalance after insertion
  void remove_fixup(RBTreeNode<T>* x,
                    RBTreeNode<T>* parent);  // rebalance after removement
};

/********************************************************************************/

template <typename T>
void RBTreeNode<T>::setColor(TreeNode<T>* node, Color newcolor) {
  auto node_rbt = static_cast<RBTreeNode<T>*>(node);
  if (!node_rbt)
    return;
  else if (node_rbt->color != newcolor)
    node_rbt->color = newcolor;
}

template <typename T>
Color RBTreeNode<T>::getColor(TreeNode<T>* node) {
  if (!node) return Color::BLACK;

  auto node_rbt = static_cast<RBTreeNode<T>*>(node);
  return node_rbt->color;
}

template <typename T>
std::unique_ptr<RBTreeNode<T>> RBTreeNode<T>::makeRBTreeNode(std::string& s,
                                                             TreeNode<T>* p) {
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

template <typename T, typename Compare>
void RedBlackTree<T, Compare>::insert_fixup(RBTreeNode<T>* z) {
  if (z == this->root_.get() || z->parent == this->root_.get()) {
    if (RBTreeNode<T>::getColor(this->root_.get()) == Color::RED) {
      RBTreeNode<T>::setColor(this->root_.get(), Color::BLACK);
    }
    return;
  }
  if (RBTreeNode<T>::getColor(z) == Color::RED &&
      RBTreeNode<T>::getColor(z->parent) == Color::RED) {
    auto zGrandparent = z->parent->parent;
    if (zGrandparent->left.get() == z->parent) /* Left */ {
      auto zUncle = zGrandparent->right.get();
      if (RBTreeNode<T>::getColor(zUncle) == Color::RED) /* uncle is red */ {
        RBTreeNode<T>::setColor(zUncle, Color::BLACK);
        RBTreeNode<T>::setColor(z->parent, Color::BLACK);
        RBTreeNode<T>::setColor(zGrandparent, Color::RED);
        insert_fixup(static_cast<RBTreeNode<T>*>(zGrandparent));
      } else if (zGrandparent->left->right.get() == z) /* case 2: LR */ {
        BinarySearchTree<T, Compare>::rotate_left(z->parent);
        insert_fixup(static_cast<RBTreeNode<T>*>(z->left.get()));
      } else /* case 3: LL */ {
        RBTreeNode<T>::setColor(zGrandparent, Color::RED);
        RBTreeNode<T>::setColor(zGrandparent->left.get(), Color::BLACK);
        BinarySearchTree<T, Compare>::rotate_right(zGrandparent);
        insert_fixup(z);
      }
    } else /* Right */ {
      auto zUncle = zGrandparent->left.get();
      if (RBTreeNode<T>::getColor(zUncle) == Color::RED) /* case 1 */ {
        RBTreeNode<T>::setColor(z->parent, Color::BLACK);
        RBTreeNode<T>::setColor(zUncle, Color::BLACK);
        RBTreeNode<T>::setColor(zGrandparent, Color::RED);
        insert_fixup(static_cast<RBTreeNode<T>*>(zGrandparent));
      } else if /* case 2 */ (zGrandparent->right &&
                              zGrandparent->right->left.get() == z) {
        BinarySearchTree<T, Compare>::rotate_right(z->parent);
        insert_fixup(static_cast<RBTreeNode<T>>(z->right.get()));
      } else /* case 3 */ {
        RBTreeNode<T>::setColor(z->parent, Color::BLACK);
        RBTreeNode<T>::setColor(zGrandparent, Color::RED);
        BinarySearchTree<T, Compare>::rotate_left(zGrandparent);
        insert_fixup(z);
      }
    }
  } else {
    if (RBTreeNode<T>::getColor(this->root_.get()) == Color::RED) {
      RBTreeNode<T>::setColor(this->root_.get(), Color::BLACK);
    }
    return;
  }
}

template <typename T, typename Compare>
void RedBlackTree<T, Compare>::remove_fixup(RBTreeNode<T>* x,
                                            RBTreeNode<T>* parent) {
  RBTreeNode<T>* w;
  while (x != this->root_.get() && RBTreeNode<T>::getColor(x) == Color::BLACK) {
    if (x == parent->left.get()) {
      w = static_cast<RBTreeNode<T>>(parent->right.get());
      if (RBTreeNode<T>::getColor(w) == Color::RED) {
        RBTreeNode<T>::setColor(w, Color::BLACK);
        RBTreeNode<T>::setColor(parent, Color::RED);
        BinarySearchTree<T, Compare>::rotate_left(parent);
        w = static_cast<RBTreeNode<T>*>(parent->right.get());
      } else if (RBTreeNode<T>::getColor(w->left.get()) == Color::BLACK &&
                 RBTreeNode<T>::getColor(w->right.get()) == Color::BLACK) {
        RBTreeNode<T>::setColor(w, Color::RED);
        x = static_cast<RBTreeNode<T>*>(parent);
        parent = static_cast<RBTreeNode<T>*>(parent->parent);
      } else {
        if (RBTreeNode<T>::getColor(w->right.get()) == Color::BLACK &&
            RBTreeNode<T>::getColor(w->left.get()) == Color::RED) {
          RBTreeNode<T>::setColor(w->left.get(), Color::BLACK);
          RBTreeNode<T>::setColor(w, Color::RED);
          BinarySearchTree<T, Compare>::rotate_right(w);
          w = static_cast<RBTreeNode<T>*>(parent->right.get());
        }

        RBTreeNode<T>::setColor(w, RBTreeNode<T>::getColor(parent));
        RBTreeNode<T>::setColor(parent, Color::BLACK);
        RBTreeNode<T>::setColor(w->right.get(), Color::BLACK);
        BinarySearchTree<T, Compare>::rotate_left(parent);
        if (RBTreeNode<T>::getColor(this->root_.get()) == Color::RED)
          RBTreeNode<T>::setColor(this->root_.get(), Color::BLACK);
        x = static_cast<RBTreeNode<T>*>(this->root_.get());
      }
    } else {
      w = static_cast<RBTreeNode<T>*>(parent->left.get());
      if (RBTreeNode<T>::getColor(w) == Color::RED) {
        RBTreeNode<T>::setColor(w, Color::BLACK);
        RBTreeNode<T>::setColor(parent, Color::RED);
        BinarySearchTree<T, Compare>::rotate_right(parent);
        w = static_cast<RBTreeNode<T>*>(parent->left.get());
      } else if (RBTreeNode<T>::getColor(w->left.get()) == Color::BLACK &&
                 RBTreeNode<T>::getColor(w->right.get()) == Color::BLACK) {
        RBTreeNode<T>::setColor(w, Color::RED);
        x = parent;
        parent = static_cast<RBTreeNode<T>*>(x->parent);
      } else {
        if (RBTreeNode<T>::getColor(w->right.get()) == Color::RED &&
            RBTreeNode<T>::getColor(w->left.get()) == Color::BLACK) {
          RBTreeNode<T>::setColor(w->right.get(), Color::BLACK);
          RBTreeNode<T>::setColor(w, Color::RED);
          BinarySearchTree<T, Compare>::rotate_left(w);
          w = static_cast<RBTreeNode<T>*>(parent->left.get());
        }

        RBTreeNode<T>::setColor(w, RBTreeNode<T>::getColor(parent));
        RBTreeNode<T>::setColor(parent, Color::BLACK);
        RBTreeNode<T>::setColor(w->left.get(), Color::BLACK);
        BinarySearchTree<T, Compare>::rotate_right(parent);
        x = static_cast<RBTreeNode<T>*>(this->root_.get());
      }
    }
  }
  RBTreeNode<T>::setColor(x, Color::BLACK);
}

template <typename T, typename Compare>
T* RedBlackTree<T, Compare>::insert(T data) {
  RBTreeNode* AlloNode = new RBTreeNode<T>(data);

  RBTreeNode<T>* newNode = static_cast<RBTreeNode<T>*>(
      BinarySearchTree<T, Compare>::insert(AlloNode));

  insert_fixup(newNode);
  return &newNode->data;
}

template <typename T, typename Compare>
template <typename K, typename... Args>
T* RedBlackTree<T, Compare>::insert_emplace(K data, Args&&... args) {
  Compare comp;
  if (!this->root_) {
    T new_value(std::forward<Args>(args)...);
    RBTreeNode<T>* node = new RBTreeNode<T>(new_value, Color::BLACK);
    this->leftmost_node_ = node;
    this->root_.reset(node);
    return &node->data;
  }

  TreeNode<T>* parent = this->root_.get();
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
  RBTreeNode<T>* z =
      static_cast<RBTreeNode<T>*>(BinarySearchTree<T, Compare>::find_node(key));
  if (!z) return;

  if (z == this->leftmost_node_) {
    if (this->leftmost_node_->right) {
      this->leftmost_node_ = this->leftmost_node_->right;
      while (this->leftmost_node_->left) {
        this->leftmost_node_ = this->leftmost_node_->left;
      }
    } else {
      this->leftmost_node_ = this->leftmost_node_->parent;
    }
  }

  std::shared_ptr<RBTreeNode<T>> y = z;
  Color y_original_color = RBTreeNode<T>::getColor(y);
  std::shared_ptr<RBTreeNode<T>> x_parent;
  std::shared_ptr<RBTreeNode<T>> x;
  if (!z->left) {
    x = std::dynamic_pointer_cast<RBTreeNode<T>>(z->right);
    x_parent = std::dynamic_pointer_cast<RBTreeNode<T>>(z->parent);
    BinarySearchTree<T, Compare>::transplant(z, z->right);
  } else if (!z->right) {
    x = std::dynamic_pointer_cast<RBTreeNode<T>>(z->left);
    x_parent = std::dynamic_pointer_cast<RBTreeNode<T>>(z->parent);
    BinarySearchTree<T, Compare>::transplant(z, z->left);
  } else {
    y = std::dynamic_pointer_cast<RBTreeNode<T>>(z->right);
    while (y->left) {
      y = std::dynamic_pointer_cast<RBTreeNode<T>>(y->left);
    }
    y_original_color = RBTreeNode<T>::getColor(y);
    x = std::dynamic_pointer_cast<RBTreeNode<T>>(y->right);
    if (y->parent == z) {
      x_parent = y;
    } else {
      x_parent = std::dynamic_pointer_cast<RBTreeNode<T>>(y->parent);
      BinarySearchTree<T, Compare>::transplant(y, x);
      y->right = z->right;
      if (y->right) y->right->parent = y;
    }
    BinarySearchTree<T, Compare>::transplant(z, y);
    y->left = z->left;
    if (y->left) y->left->parent = y;
    RBTreeNode<T>::setColor(y, RBTreeNode<T>::getColor(z));
  }

  if (y_original_color == Color::BLACK) remove_fixup(x, x_parent);
}

#endif