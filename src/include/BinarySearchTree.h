#ifndef __BINARYSEARCHTREE_H__
#define __BINARYSEARCHTREE_H__

#include <functional>
#include <memory>
#include <string>

template <typename T> struct TreeNode {
  T value;
  std::shared_ptr<TreeNode<T>> parent;
  std::shared_ptr<TreeNode<T>> left;
  std::shared_ptr<TreeNode<T>> right;
  TreeNode(T v) : TreeNode(v, nullptr) {}
  TreeNode(T v, std::shared_ptr<TreeNode<T>> p)
      : value(v), left(nullptr), right(nullptr), parent(p) {}
  virtual ~TreeNode() = default;
};

template <typename T, typename Compare = std::less<>> class BinarySearchTree {
public:
  BinarySearchTree() : root(nullptr) {};
  virtual ~BinarySearchTree();

  virtual void insert(T value);
  virtual void remove(T value);

  // virtual std::string serialize();
  // virtual void deserialize(const std::string& code);
  virtual std::string inorder();

  std::shared_ptr<TreeNode<T>> find_node(T value);
  void insert(std::shared_ptr<TreeNode<T>> node);

  void rotate_left(std::shared_ptr<TreeNode<T>> x);
  void rotate_right(std::shared_ptr<TreeNode<T>> x);

protected:
  void transplant(std::shared_ptr<TreeNode<T>> u,
                  std::shared_ptr<TreeNode<T>> v);
  virtual void inorder_helper(std::shared_ptr<TreeNode<T>> subroot,
                              std::stringstream &str, bool& isFirst);

  std::shared_ptr<TreeNode<T>> root;
};

/*********************************************************************************/

#include <functional>
#include <stdexcept>

template <typename T> static void clear(std::shared_ptr<TreeNode<T>> node) {
  if (!node)
    return;
  clear(node->left);
  clear(node->right);
  node->left = nullptr;
  node->right = nullptr;
  node->parent = nullptr;
}

template <typename T, typename Compare>
BinarySearchTree<T, Compare>::~BinarySearchTree() {
  clear(this->root);
}

template <typename T, typename Compare>
std::shared_ptr<TreeNode<T>> BinarySearchTree<T, Compare>::find_node(T value) {
  Compare comp;
  std::shared_ptr<TreeNode<T>> node = root;
  while (node && node->value != value) {
    if (comp(value, node->value)) {
      node = node->left;
    } else {
      node = node->right;
    }
  }

  return node;
}

template <typename T, typename Compare>
void BinarySearchTree<T, Compare>::insert(std::shared_ptr<TreeNode<T>> node) {
  Compare comp;
  if (!this->root) {
    this->root = node;
    return;
  }
  std::shared_ptr<TreeNode<T>> parent = root;
  while (parent) {
    if (comp(node->value, parent->value)) {
      if (parent->left) {
        parent = parent->left;
      } else {
        parent->left = node;
        node->parent = parent;
        break;
      }
    } else {
      if (parent->right) {
        parent = parent->right;
      } else {
        parent->right = node;
        node->parent = parent;
        break;
      }
    }
  }
}

template <typename T, typename Compare>
void BinarySearchTree<T, Compare>::insert(T value) {
  insert(std::make_shared<TreeNode<T>>(value));
}

template <typename T, typename Compare>
void BinarySearchTree<T, Compare>::transplant(std::shared_ptr<TreeNode<T>> u,
                                              std::shared_ptr<TreeNode<T>> v) {
  if (u->parent == nullptr)
    root = v;
  else if (u->parent->left == u)
    u->parent->left = v;
  else
    u->parent->right = v;
  if (v)
    v->parent = u->parent;
}

template <typename T, typename Compare>
void BinarySearchTree<T, Compare>::remove(T value) {
  std::shared_ptr<TreeNode<T>> tar = find_node(value);
  if (!tar)
    return;
  if (tar->left && tar->right) {
    std::shared_ptr<TreeNode<T>> substituteNode = tar->right;
    while (substituteNode->left)
      substituteNode = substituteNode->left;
    transplant(substituteNode, substituteNode->right);
    substituteNode->left = tar->left;
    substituteNode->right = tar->right;
    transplant(tar, substituteNode);
    tar->left = nullptr;
    tar->right = nullptr;
    tar->parent = nullptr;
    if (substituteNode->left)
      substituteNode->left->parent = substituteNode;
    if (substituteNode->right)
      substituteNode->right->parent = substituteNode;
  } else if (tar->left) {
    transplant(tar, tar->left);
  } else {
    transplant(tar, tar->right);
  }
}

template <typename T, typename Compare>
void BinarySearchTree<T, Compare>::rotate_left(std::shared_ptr<TreeNode<T>> x) {
  if (!x || !x->right)
    return;

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
  if (!y || !y->left)
    return;

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
  if (root) {
    std::stringstream inorderString;
    bool isFirst = true;
    inorder_helper(root, inorderString, isFirst);
    return inorderString.str();
  } else {
    return "";
  }
}

template <typename T, typename Compare>
void BinarySearchTree<T, Compare>::inorder_helper(
    std::shared_ptr<TreeNode<T>> subroot, std::stringstream &str,
    bool& isFirst) {

  if (!subroot)
    return;
  else {
    inorder_helper(subroot->left, str, isFirst);
    if (isFirst) {
      isFirst = false;
      str << subroot->value;
    } else
      str << ' ' << subroot->value;
    inorder_helper(subroot->right, str, isFirst);
  }
}

/*

#include <sstream>
#include <queue>

template <typename T, typename Compare>
std::string BinarySearchTree<T, Compare>::serialize() {
    std::ostringstream out;
    bool isFirst = true;
    std::queue<std::shared_ptr<TreeNode<T>>> q;
    int nonNullNode = 0;

    if (root) {
        q.push(root);
        nonNullNode++;
    } else {
        return "#";
    }

    while (nonNullNode) {
        std::shared_ptr<TreeNode<T>> node = q.front();
        q.pop();
        if (node) {
            nonNullNode--;
            if (isFirst) {
                out << node->value;
                isFirst = false;
            } else {
                out << ' ' << node->value;
            }
            if (node->left)
                nonNullNode++;
            if (node->right)
                nonNullNode++;
            q.push(node->left);
            q.push(node->right);
        } else {
            if (isFirst) {
                out << "#";
                break;
            } else {
                out << " #";
            }
        }
    }

    return out.str();
}


template <typename T, typename Compare>
void BinarySearchTree<T, Compare>::deserialize(const std::string& code) {
    std::stringstream ss(code);
    std::string token;
    ss >> token;
    if (token != "#") {
        root = std::make_shared<TreeNode<T>>(stoi(token));
    } else {
        return;
    }
    std::queue<std::shared_ptr<TreeNode<T>>> q;
    q.push(root);
    while (!q.empty()) {
        std::shared_ptr<TreeNode<T>> node = q.front();
        q.pop();

        if (ss >> token) {
            if (token != "#") {
                std::shared_ptr<TreeNode<T>> newNode =
std::make_shared<TreeNode<T>>(stoi(token)); node->left = newNode;
                newNode->parent = node;
            }
        } else {
            break;
        }

        if (ss >> token) {
            if (token != "#") {
                std::shared_ptr<TreeNode<T>> newNode =
std::make_shared<TreeNode<T>>(stoi(token)); node->right = newNode;
                newNode->parent = node;
            }
        } else {
            break;
        }

        if (node->left)
            q.push(node->left);
        if (node->right)
            q.push(node->right);
    }
}

*/

#endif