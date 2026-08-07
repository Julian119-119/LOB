#ifndef __BINARYSEARCHTREE_HPP__
#define __BINARYSEARCHTREE_HPP__

#include <functional>
#include <memory>
#include <sstream>

template <typename T>
struct TreeNode {
  T data;
  TreeNode<T>* parent = nullptr;
  std::unique_ptr<TreeNode<T>> left;
  std::unique_ptr<TreeNode<T>> right;

  template <typename... Args>
  TreeNode(T v) : data(v), parent(nullptr) {}
  TreeNode(T v, TreeNode<T>* p) : data(v), parent(p) {}
  virtual ~TreeNode() = default;
};

template <typename T, typename Compare = std::less<>>
class BinarySearchTree {
 public:
  BinarySearchTree() : leftmost_node_(nullptr) {};
  virtual ~BinarySearchTree();

  bool empty() const { return root_ == nullptr; }
  template <typename K>
  TreeNode<T>* find_node(const K& data);
  TreeNode<T>* get_leftmost_node() const { return leftmost_node_; }

  TreeNode<T>* get_successor(
      TreeNode<T>* node) const;  // get the next node in in-order

  TreeNode<T>* insert(T data);
  TreeNode<T>* insert(TreeNode<T>* node);
  void remove(T data);

  std::string inorder();

  void rotate_left(TreeNode<T>* x);
  void rotate_right(TreeNode<T>* x);

 protected:
  // 用來將 v 頂替 u，但只處理「父子連結」，不處理 left and right subtree
  // 回傳暫存 u 的 unique_ptr
  std::unique_ptr<TreeNode<T>> transplant(TreeNode<T>* u,
                             TreeNode<T>* v);  
  // 為了 transplant 使用 std::move() ，指向 n 的 unique_ptr，並回傳
  // reference，使它可以用 std::move()
  std::unique_ptr<TreeNode<T>>& owning_slot(TreeNode<T>* n);
  void inorder_helper(TreeNode<T>* subroot, std::stringstream& str,
                      bool& isFirst);

  std::unique_ptr<TreeNode<T>> root_;
  TreeNode<T>* leftmost_node_;
};

/*********************************************************************************/

#include <functional>
#include <stdexcept>

template <typename T, typename Compare>
BinarySearchTree<T, Compare>::~BinarySearchTree() {
  root_.reset(nullptr);
  leftmost_node_ = nullptr;
}

template <typename T, typename Compare>
template <typename K>
/* find_node: herogeneously find */
TreeNode<T>* BinarySearchTree<T, Compare>::find_node(const K& key) {
  // 確保 O(1) 時間內可以找到最小的 node
  Compare comp;
  if (leftmost_node_ &&
      (!comp(leftmost_node_->data, key) && !comp(key, leftmost_node_->data))) {
    return leftmost_node_;
  }

  TreeNode<T>* node = root_.get();
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
TreeNode<T>* BinarySearchTree<T, Compare>::get_successor(
    TreeNode<T>* node) const {
  if (!node) return nullptr;

  TreeNode<T>* curr = node;
  if (curr->right) {
    curr = curr->right.get();
    while (curr->left) curr = curr->left.get();
  } else {
    TreeNode<T>* p = curr->parent;
    while (p && p->left.get() != curr) {
      curr = p;
      p = p->parent;
    }
    curr = p;
  }
  return curr;
}

template <typename T, typename Compare>
TreeNode<T>* BinarySearchTree<T, Compare>::insert(TreeNode<T>* node) {
  Compare comp;
  if (!root_) {
    root_.reset(node);
    leftmost_node_ = node;
    return node;
  }

  TreeNode<T>* parent = root_.get();
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
        return node;
      }
    } else if (comp(parent->data, node->data)) {
      if (parent->right) {
        parent = parent->right;
      } else {
        parent->right.reset(node);
        node->parent = parent;
        return node;
      }
    } else {
      return parent;
    }
  }

  return nullptr;
}

template <typename T, typename Compare>
TreeNode<T>* BinarySearchTree<T, Compare>::insert(T data) {
  return insert(new TreeNode(data));
}

template <typename T, typename Compare>
std::unique_ptr<TreeNode<T>> BinarySearchTree<T, Compare>::transplant(TreeNode<T>* u, TreeNode<T>* v) {
  // 將 v 給儲存 u 的 unique_ptr 與將 u 給暫存的 unique_ptr (old)
  std::unique_ptr<TreeNode<T>>& slot = owning_slot(u);
  std::unique_ptr<TreeNode<T>> old = std::move(u);
  slot = std::move(v);
  // 更改向上的路線
  if (v) v->parent = u->parent;
  return slot;
}

template <typename T, typename Compare>
std::unique_ptr<TreeNode<T>>& BinarySearchTree<T, Compare>::owning_slot(TreeNode<T>* n) {
  if (!n->parent)  return root_
  return (n->parent->left.get() == n) ? n->parent->left : n->parent->right;
}


template <typename T, typename Compare>
void BinarySearchTree<T, Compare>::remove(T data) {
  TreeNode<T>* tar = find_node(data);
  if (!tar) return;

  // 重新設定 leftmost_node_
  if (tar == leftmost_node_) {
    if (leftmost_node_->right) {
      leftmost_node_ = leftmost_node_->right.get();
      while (leftmost_node_->left) {
        leftmost_node_ = leftmost_node_->left.get();
      }
    } else {
      leftmost_node_ = leftmost_node_->parent;
    }
  }

  if (tar->left && tar->right) {
    TreeNode<T>* substituteNode = tar->right.get();
    while (substituteNode->left) substituteNode = substituteNode->left.get();
    transplant(substituteNode, substituteNode->right);
    substituteNode->left = tar->left;
    substituteNode->right = tar->right;
    transplant(tar, substituteNode);
    tar->left = nullptr;
    tar->right = nullptr;
    tar->parent = nullptr;
    if (substituteNode->left) substituteNode->left->parent = substituteNode;
    if (substituteNode->right) substituteNode->right->parent = substituteNode;
  } else if (tar->left) {
    transplant(tar, tar->left);
  } else {
    transplant(tar, tar->right);
  }
}

template <typename T, typename Compare>
void BinarySearchTree<T, Compare>::rotate_left(std::shared_ptr<TreeNode<T>> x) {
  if (!x || !x->right) return;

  std::shared_ptr<TreeNode<T>> substituteNode = x->right;
  transplant(x, substituteNode);
  x->right = nullptr;
  if (substituteNode->left) {
    std::shared_ptr<TreeNode<T>> leftSubtree = substituteNode->left;
    substituteNode->left = x;
    leftSubtree->parent = x;
    x->right = leftSubtree;
    x->parent = substituteNode;
  } else {
    substituteNode->left = x;
    x->parent = substituteNode;
  }
}

template <typename T, typename Compare>
void BinarySearchTree<T, Compare>::rotate_right(
    std::shared_ptr<TreeNode<T>> y) {
  if (!y || !y->left) return;

  std::shared_ptr<TreeNode<T>> substituteNode = y->left;
  transplant(y, substituteNode);
  y->left = nullptr;
  if (substituteNode->right) {
    std::shared_ptr<TreeNode<T>> rightSubtree = substituteNode->right;
    substituteNode->right = y;
    y->parent = substituteNode;
    y->left = rightSubtree;
    rightSubtree->parent = y;
  } else {
    substituteNode->right = y;
    y->parent = substituteNode;
  }
}

template <typename T, typename Compare>
std::string BinarySearchTree<T, Compare>::inorder() {
  if (root_) {
    std::stringstream inorderString;
    bool isFirst = true;
    inorder_helper(root_, inorderString, isFirst);
    return inorderString.str();
  } else {
    return "";
  }
}

template <typename T, typename Compare>
void BinarySearchTree<T, Compare>::inorder_helper(
    std::shared_ptr<TreeNode<T>> subroot, std::stringstream& str,
    bool& isFirst) {
  if (!subroot)
    return;
  else {
    inorder_helper(subroot->left, str, isFirst);
    if (isFirst) {
      isFirst = false;
      str << subroot->data;
    } else
      str << ' ' << subroot->data;
    inorder_helper(subroot->right, str, isFirst);
  }
}

#endif