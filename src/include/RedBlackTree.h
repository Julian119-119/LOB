#ifndef __REDBLACKTREE_H__
#define __REDBLACKTREE_H__

#include "BinarySearchTree.h"

enum class Color {RED, BLACK};

template <typename T>
struct RBTreeNode : TreeNode<T> {
    Color color;
    RBTreeNode<T>(T v): color(Color::RED), TreeNode<T>(v) {}
    RBTreeNode<T>(T v, Color c): color(c), TreeNode<T>(v) {}
    RBTreeNode<T>(T v, Color c, std::shared_ptr<TreeNode<T>> p): color(c), TreeNode<T>(v, p) {}
    ~RBTreeNode<T>() = default;
    
    static std::shared_ptr<RBTreeNode<T>> makeRBTreeNode(std::string& s, std::shared_ptr<TreeNode<T>> p = nullptr);
    static Color getColor(std::shared_ptr<TreeNode<T>> node);
    static void setColor(std::shared_ptr<TreeNode<T>> node, Color newcolor);
};


template <typename T, typename Compare=std::less<>>
class RedBlackTree : public BinarySearchTree<T, Compare> {
public:
    RedBlackTree<T, Compare>() : BinarySearchTree<T, Compare>() {};
    std::shared_ptr<TreeNode<T>> insert(T data);
    void remove(T data);

private:
    void insert_fixup(std::shared_ptr<RBTreeNode<T>> z);
    void remove_fixup(std::shared_ptr<RBTreeNode<T>> x, std::shared_ptr<RBTreeNode<T>> parent);
};

/********************************************************************************/

template <typename T>
void RBTreeNode<T>::setColor(std::shared_ptr<TreeNode<T>> node, Color newcolor) {
  auto node_rbt = std::dynamic_pointer_cast<RBTreeNode<T>>(node);
  if (!node_rbt)
    return;
  else if (node_rbt->color != newcolor)
    node_rbt->color = newcolor;
}

template <typename T>
Color RBTreeNode<T>::getColor(std::shared_ptr<TreeNode<T>> node) {
  if (!node)
    return Color::BLACK;

  auto node_rbt = std::dynamic_pointer_cast<RBTreeNode<T>>(node);
  return node_rbt->color;
}

template <typename T>
std::shared_ptr<RBTreeNode<T>> RBTreeNode<T>::makeRBTreeNode(std::string &s,
                                                  std::shared_ptr<TreeNode<T>> p) {
  if (s == "#") {
    return nullptr;
  } else if (s[0] == '(') {
    s.pop_back();
    s[0] = ' ';
    return std::make_shared<RBTreeNode<T>>(stoi(s), Color::RED, p);
  } else {
    return std::make_shared<RBTreeNode<T>>(stoi(s), Color::BLACK, p);
  }
}

template <typename T, typename Compare>
void RedBlackTree<T, Compare>::insert_fixup(std::shared_ptr<RBTreeNode<T>> z) {
  if (z == this->root || z->parent == this->root) {
    if (RBTreeNode<T>::getColor(this->root) == Color::RED) {
      RBTreeNode<T>::setColor(this->root, Color::BLACK);
    }
    return;
  }
  if (RBTreeNode<T>::getColor(z) == Color::RED &&
      RBTreeNode<T>::getColor(z->parent) == Color::RED) {
    auto zGrandparent = z->parent->parent;
    if (zGrandparent->left == z->parent) /* Left */ {
      auto zUncle = zGrandparent->right;
      if (RBTreeNode<T>::getColor(zUncle) == Color::RED) /* uncle is red */ {
        RBTreeNode<T>::setColor(zUncle, Color::BLACK);
        RBTreeNode<T>::setColor(z->parent, Color::BLACK);
        RBTreeNode<T>::setColor(zGrandparent, Color::RED);
        insert_fixup(std::dynamic_pointer_cast<RBTreeNode<T>>(zGrandparent));
      } else if (zGrandparent->left->right == z) /* case 2: LR */ {
        BinarySearchTree<T, Compare>::rotate_left(z->parent);
        insert_fixup(std::dynamic_pointer_cast<RBTreeNode<T>>(z->left));
      } else /* case 3: LL */ {
        RBTreeNode<T>::setColor(zGrandparent, Color::RED);
        RBTreeNode<T>::setColor(zGrandparent->left, Color::BLACK);
        BinarySearchTree<T, Compare>::rotate_right(zGrandparent);
        insert_fixup(z);
      }
    } else /* Right */ {
      auto zUncle = zGrandparent->left;
      if (RBTreeNode<T>::getColor(zUncle) == Color::RED) /* case 1 */ {
        RBTreeNode<T>::setColor(z->parent, Color::BLACK);
        RBTreeNode<T>::setColor(zUncle, Color::BLACK);
        RBTreeNode<T>::setColor(zGrandparent, Color::RED);
        insert_fixup(std::dynamic_pointer_cast<RBTreeNode<T>>(zGrandparent));
      } else if /* case 2 */ (zGrandparent->right &&
                              zGrandparent->right->left == z) {
        BinarySearchTree<T, Compare>::rotate_right(z->parent);
        insert_fixup(std::dynamic_pointer_cast<RBTreeNode<T>>(z->right));
      } else /* case 3 */ {
        RBTreeNode<T>::setColor(z->parent, Color::BLACK);
        RBTreeNode<T>::setColor(zGrandparent, Color::RED);
        BinarySearchTree<T, Compare>::rotate_left(zGrandparent);
        insert_fixup(z);
      }
    }
  } else {
    if (RBTreeNode<T>::getColor(this->root) == Color::RED) {
      RBTreeNode<T>::setColor(this->root, Color::BLACK);
    }
    return;
  }
}

template <typename T, typename Compare>
void RedBlackTree<T, Compare>::remove_fixup(std::shared_ptr<RBTreeNode<T>> x,
                                std::shared_ptr<RBTreeNode<T>> parent) {

  std::shared_ptr<RBTreeNode<T>> w;
  while (x != this->root && RBTreeNode<T>::getColor(x) == Color::BLACK) {
    if (x == parent->left) {
      w = std::dynamic_pointer_cast<RBTreeNode<T>>(parent->right);
      if (RBTreeNode<T>::getColor(w) == Color::RED) {
        RBTreeNode<T>::setColor(w, Color::BLACK);
        RBTreeNode<T>::setColor(parent, Color::RED);
        BinarySearchTree<T, Compare>::rotate_left(parent);
        w = std::dynamic_pointer_cast<RBTreeNode<T>>(parent->right);
      } else if (RBTreeNode<T>::getColor(w->left) == Color::BLACK &&
                 RBTreeNode<T>::getColor(w->right) == Color::BLACK) {
        RBTreeNode<T>::setColor(w, Color::RED);
        x = std::dynamic_pointer_cast<RBTreeNode<T>>(parent);
        parent = std::dynamic_pointer_cast<RBTreeNode<T>>(parent->parent);
      } else {
        if (RBTreeNode<T>::getColor(w->right) == Color::BLACK &&
            RBTreeNode<T>::getColor(w->left) == Color::RED) {
          RBTreeNode<T>::setColor(w->left, Color::BLACK);
          RBTreeNode<T>::setColor(w, Color::RED);
          BinarySearchTree<T, Compare>::rotate_right(w);
          w = std::dynamic_pointer_cast<RBTreeNode<T>>(parent->right);
        }

        RBTreeNode<T>::setColor(w, RBTreeNode<T>::getColor(parent));
        RBTreeNode<T>::setColor(parent, Color::BLACK);
        RBTreeNode<T>::setColor(w->right, Color::BLACK);
        BinarySearchTree<T, Compare>::rotate_left(parent);
        if (RBTreeNode<T>::getColor(this->root) == Color::RED)
          RBTreeNode<T>::setColor(this->root, Color::BLACK);
        x = std::dynamic_pointer_cast<RBTreeNode<T>>(this->root);
      }
    } else {
      w = std::dynamic_pointer_cast<RBTreeNode<T>>(parent->left);
      if (RBTreeNode<T>::getColor(w) == Color::RED) {
        RBTreeNode<T>::setColor(w, Color::BLACK);
        RBTreeNode<T>::setColor(parent, Color::RED);
        BinarySearchTree<T, Compare>::rotate_right(parent);
        w = std::dynamic_pointer_cast<RBTreeNode<T>>(parent->left);
      } else if (RBTreeNode<T>::getColor(w->left) == Color::BLACK &&
                 RBTreeNode<T>::getColor(w->right) == Color::BLACK) {
        RBTreeNode<T>::setColor(w, Color::RED);
        x = parent;
        parent = std::dynamic_pointer_cast<RBTreeNode<T>>(x->parent);
      } else {
        if (RBTreeNode<T>::getColor(w->right) == Color::RED &&
            RBTreeNode<T>::getColor(w->left) == Color::BLACK) {
          RBTreeNode<T>::setColor(w->right, Color::BLACK);
          RBTreeNode<T>::setColor(w, Color::RED);
          BinarySearchTree<T, Compare>::rotate_left(w);
          w = std::dynamic_pointer_cast<RBTreeNode<T>>(parent->left);
        }

        RBTreeNode<T>::setColor(w, RBTreeNode<T>::getColor(parent));
        RBTreeNode<T>::setColor(parent, Color::BLACK);
        RBTreeNode<T>::setColor(w->left, Color::BLACK);
        BinarySearchTree<T, Compare>::rotate_right(parent);
        x = std::dynamic_pointer_cast<RBTreeNode<T>>(this->root);
      }
    }
  }
  RBTreeNode<T>::setColor(x, Color::BLACK);
}

template <typename T, typename Compare>
std::shared_ptr<TreeNode<T>> RedBlackTree<T, Compare>::insert(T data) {
  std::shared_ptr<RBTreeNode<T>> AlloNode = std::make_shared<RBTreeNode<T>>(data);
  std::shared_ptr<RBTreeNode<T>> newNode = std::dynamic_pointer_cast<RBTreeNode<T>>(BinarySearchTree<T, Compare>::insert(AlloNode));
  insert_fixup(newNode);
  return newNode;
}

template <typename T, typename Compare>
void RedBlackTree<T, Compare>::remove(T data) {
  std::shared_ptr<RBTreeNode<T>> z =
      std::dynamic_pointer_cast<RBTreeNode<T>>(BinarySearchTree<T, Compare>::find_node(data));
  if (!z)
    return;

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

  if (y_original_color == Color::BLACK)
      remove_fixup(x, x_parent);
}

#endif