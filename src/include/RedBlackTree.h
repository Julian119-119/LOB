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
    
    // std::string toString();
    static std::shared_ptr<RBTreeNode<T>> makeRBTreeNode(std::string& s, std::shared_ptr<TreeNode<T>> p = nullptr);
    static Color getColor(std::shared_ptr<TreeNode<T>> node);
    static void setColor(std::shared_ptr<TreeNode<T>> node, Color newcolor);
};


template <typename T, typename Compare=std::less<>>
class RedBlackTree : public BinarySearchTree<T, Compare> {
public:
    RedBlackTree<T, Compare>() : BinarySearchTree<T, Compare>() {};
    void insert(T value);
    void remove(T value);
    void insert(std::shared_ptr<TreeNode<T>> node);
    // std::string serialize() override;
    // void deserialize(const std::string& code) override;

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

/*
template <typename T>
std::string RBTreeNode<T>::toString() {
  std::stringstream str;
  if (this->color == Color::RED) {
    str << '(' << this->value << ')';
  } else {
    str << '(' << this->value;
  }
  return str.str();
}
*/

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
void RedBlackTree<T, Compare>::insert(T value) {
  std::shared_ptr<RBTreeNode<T>> newNode = std::make_shared<RBTreeNode<T>>(value);
  BinarySearchTree<T, Compare>::insert(newNode);
  insert_fixup(newNode);
}

template <typename T, typename Compare>
void RedBlackTree<T, Compare>::remove(T value) {
  std::shared_ptr<RBTreeNode<T>> z =
      std::dynamic_pointer_cast<RBTreeNode<T>>(BinarySearchTree<T, Compare>::find_node(value));
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

/*
#include <queue>
#include <sstream>

std::string RedBlackTree<T, Compare>::serialize() {
  if (!this->root)
    return "#";
  std::ostringstream out;
  queue<std::shared_ptr<TreeNode<T>>> q;
  q.push(this->root);
  bool isFirst = true;
  int nonNullNode = 1;
  while (nonNullNode) {
    auto node = std::dynamic_pointer_cast<RBTreeNode<T>>(q.front());
    q.pop();
    if (node) {
      nonNullNode--;
      if (isFirst) {
        out << node->toString();
        isFirst = false;
      } else {
        out << ' ' << node->toString();
      }
      if (node->left)
        nonNullNode++;
      if (node->right)
        nonNullNode++;
      q.push(node->left);
      q.push(node->right);
    } else {
      out << " #";
    }
  }

  return out.str();
}

void RedBlackTree<T, Compare>::deserialize(const std::string &code) {
  stringstream ss(code);
  std::string token;
  ss >> token;
  this->root = RBTreeNode<T>::makeRBTreeNode(token);
  if (!this->root)
    return;
  queue<std::shared_ptr<TreeNode<T>>> q;
  q.push(this->root);
  while (1) {
    auto node = std::dynamic_pointer_cast<RBTreeNode<T>>(q.front());
    q.pop();
    if (ss >> token) {
      std::shared_ptr<RBTreeNode<T>> newNode = RBTreeNode<T>::makeRBTreeNode(token, node);
      node->left = newNode;
      if (newNode)
        q.push(newNode);
    } else {
      break;
    }
    if (ss >> token) {
      std::shared_ptr<RBTreeNode<T>> newNode = RBTreeNode<T>::makeRBTreeNode(token, node);
      node->right = newNode;
      if (newNode)
        q.push(newNode);
    } else {
      break;
    }
  }
}
*/

#endif