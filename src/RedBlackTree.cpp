#include "RedBlackTree.h"

void RBTreeNode::setColor(shared_ptr<TreeNode> node, Color newcolor) {
  auto node_rbt = dynamic_pointer_cast<RBTreeNode>(node);
  if (!node_rbt)
    return;
  else if (node_rbt->color != newcolor)
    node_rbt->color = newcolor;
}

Color RBTreeNode::getColor(shared_ptr<TreeNode> node) {
  if (!node)
    return Color::BLACK;

  auto node_rbt = dynamic_pointer_cast<RBTreeNode>(node);
  return node_rbt->color;
}

string RBTreeNode::toString() {
  string str;
  if (this->color == Color::RED) {
    str += "(" + to_string(this->value) + ")";
  } else {
    str += to_string(this->value);
  }
  return str;
}

shared_ptr<RBTreeNode> RBTreeNode::makeRBTreeNode(string &s,
                                                  shared_ptr<TreeNode> p) {
  if (s == "#") {
    return nullptr;
  } else if (s[0] == '(') {
    s.pop_back();
    s[0] = ' ';
    return make_shared<RBTreeNode>(stoi(s), Color::RED, p);
  } else {
    return make_shared<RBTreeNode>(stoi(s), Color::BLACK, p);
  }
}

void RedBlackTree::insert_fixup(shared_ptr<RBTreeNode> z) {
  // Todo: finish this
  /*
      RB-INSERT-FIXUP(T, z)
          while z.p.color == RED
              if z.p == z.p.p.left
                  y = z.p.p.right
                  if y.color == RED
                      z.p.color = BLACK
                      y.color = BLACK
                      z.p.p.color = RED
                      z = z.p.p
                  else
                      if z == z.p.right
                          z = z.p
                          LEFT-ROTATE(T, z)
                      z.p.color = BLACK
                      z.p.p.color = RED
                      RIGHT-ROTATE(T, z.p.p)
              else
                  (same as 'then' clause with "right" and "left" exchanged)

          T.root.color = BLACK
  */
  // cerr << "'insert_fixup' begin" << endl;
  if (z == root || z->parent == root) {
    if (RBTreeNode::getColor(root) == Color::RED) {
      RBTreeNode::setColor(root, Color::BLACK);
    }
    return;
  }
  if (RBTreeNode::getColor(z) == Color::RED &&
      RBTreeNode::getColor(z->parent) == Color::RED) {
    auto zGrandparent = z->parent->parent;
    if (zGrandparent->left == z->parent) /* Left */ {
      auto zUncle = zGrandparent->right;
      if (RBTreeNode::getColor(zUncle) == Color::RED) /* uncle is red */ {
        RBTreeNode::setColor(zUncle, Color::BLACK);
        RBTreeNode::setColor(z->parent, Color::BLACK);
        RBTreeNode::setColor(zGrandparent, Color::RED);
        insert_fixup(dynamic_pointer_cast<RBTreeNode>(zGrandparent));
      } else if (zGrandparent->left->right == z) /* case 2: LR */ {
        BinarySearchTree::rotate_left(z->parent);
        insert_fixup(dynamic_pointer_cast<RBTreeNode>(z->left));
      } else /* case 3: LL */ {
        RBTreeNode::setColor(zGrandparent, Color::RED);
        RBTreeNode::setColor(zGrandparent->left, Color::BLACK);
        BinarySearchTree::rotate_right(zGrandparent);
        insert_fixup(z);
      }
    } else /* Right */ {
      auto zUncle = zGrandparent->left;
      if (RBTreeNode::getColor(zUncle) == Color::RED) /* case 1 */ {
        RBTreeNode::setColor(z->parent, Color::BLACK);
        RBTreeNode::setColor(zUncle, Color::BLACK);
        RBTreeNode::setColor(zGrandparent, Color::RED);
        insert_fixup(dynamic_pointer_cast<RBTreeNode>(zGrandparent));
      } else if /* case 2 */ (zGrandparent->right &&
                              zGrandparent->right->left == z) {
        BinarySearchTree::rotate_right(z->parent);
        insert_fixup(dynamic_pointer_cast<RBTreeNode>(z->right));
      } else /* case 3 */ {
        RBTreeNode::setColor(z->parent, Color::BLACK);
        RBTreeNode::setColor(zGrandparent, Color::RED);
        BinarySearchTree::rotate_left(zGrandparent);
        insert_fixup(z);
      }
    }
  } else {
    if (RBTreeNode::getColor(root) == Color::RED) {
      RBTreeNode::setColor(root, Color::BLACK);
    }
    return;
  }
}

void RedBlackTree::remove_fixup(shared_ptr<RBTreeNode> x,
                                shared_ptr<RBTreeNode> parent) {
  // Todo: finish this
  /*
      RB-DELETE-FIXUP(T, x)
          while x != T.root and x.color == BLACK
              if x == x.p.left then
                  w = x.p.right
                  if w.color == RED
                      w.color = BLACK
                      x.p.color = RED
                      LEFT-ROTATE(T, x.p)
                      w = x.p.right
                  if w.left.color == BLACK and w.right.color == BLACK
                      w.color = RED
                      x = x.p
                  else
                      if w.right.color == BLACK
                          w.left.color = BLACK
                          w.color = RED
                          RIGHT-ROTATE(T, w)
                          w = x.p.right
                      w.color = x.p.color
                      x.p.color = BLACK
                      w.right.color = BLACK
                      LEFT-ROTATE(T, x.p)
                      x = T.root
              else
                  (same as 'then' clause with "right" and "left" exchanged)
          x.color = BLACK
  */

  shared_ptr<RBTreeNode> w;
  while (x != root && RBTreeNode::getColor(x) == Color::BLACK) {
    if (x == parent->left) {
      w = dynamic_pointer_cast<RBTreeNode>(parent->right);
      if (RBTreeNode::getColor(w) == Color::RED) {
        RBTreeNode::setColor(w, Color::BLACK);
        RBTreeNode::setColor(parent, Color::RED);
        BinarySearchTree::rotate_left(parent);
        w = dynamic_pointer_cast<RBTreeNode>(parent->right);
      } else if (RBTreeNode::getColor(w->left) == Color::BLACK &&
                 RBTreeNode::getColor(w->right) == Color::BLACK) {
        RBTreeNode::setColor(w, Color::RED);
        x = dynamic_pointer_cast<RBTreeNode>(parent);
        parent = dynamic_pointer_cast<RBTreeNode>(parent->parent);
      } else {
        if (RBTreeNode::getColor(w->right) == Color::BLACK &&
            RBTreeNode::getColor(w->left) == Color::RED) {
          RBTreeNode::setColor(w->left, Color::BLACK);
          RBTreeNode::setColor(w, Color::RED);
          BinarySearchTree::rotate_right(w);
          w = dynamic_pointer_cast<RBTreeNode>(parent->right);
        }

        RBTreeNode::setColor(w, RBTreeNode::getColor(parent));
        RBTreeNode::setColor(parent, Color::BLACK);
        RBTreeNode::setColor(w->right, Color::BLACK);
        BinarySearchTree::rotate_left(parent);
        if (RBTreeNode::getColor(root) == Color::RED)
          RBTreeNode::setColor(root, Color::BLACK);
        x = dynamic_pointer_cast<RBTreeNode>(root);
      }
    } else {
      w = dynamic_pointer_cast<RBTreeNode>(parent->left);
      if (RBTreeNode::getColor(w) == Color::RED) {
        RBTreeNode::setColor(w, Color::BLACK);
        RBTreeNode::setColor(parent, Color::RED);
        BinarySearchTree::rotate_right(parent);
        w = dynamic_pointer_cast<RBTreeNode>(parent->left);
      } else if (RBTreeNode::getColor(w->left) == Color::BLACK &&
                 RBTreeNode::getColor(w->right) == Color::BLACK) {
        RBTreeNode::setColor(w, Color::RED);
        x = parent;
        parent = dynamic_pointer_cast<RBTreeNode>(x->parent);
      } else {
        if (RBTreeNode::getColor(w->right) == Color::RED &&
            RBTreeNode::getColor(w->left) == Color::BLACK) {
          RBTreeNode::setColor(w->right, Color::BLACK);
          RBTreeNode::setColor(w, Color::RED);
          BinarySearchTree::rotate_left(w);
          w = dynamic_pointer_cast<RBTreeNode>(parent->left);
        }

        RBTreeNode::setColor(w, RBTreeNode::getColor(parent));
        RBTreeNode::setColor(parent, Color::BLACK);
        RBTreeNode::setColor(w->left, Color::BLACK);
        BinarySearchTree::rotate_right(parent);
        x = dynamic_pointer_cast<RBTreeNode>(root);
      }
    }
  }
  RBTreeNode::setColor(x, Color::BLACK);
}

void RedBlackTree::insert(int value) {
  // Todo: finish this
  /*
      RB-INSERT(T, z)
          BST-INSERT(T, z)
          z.color = RED
          RB-INSERT-FIXUP(z)
  */
  shared_ptr<RBTreeNode> newNode = make_shared<RBTreeNode>(value);
  BinarySearchTree::insert(newNode);
  insert_fixup(newNode);
}

void RedBlackTree::remove(int value) {
  // Todo: finish this
  /*
      RB-DELETE(T, z)
          y = z
          y-original-color = y.color
          if z.left == T.nil
              x = z.right
              RB-TRANSPLANT(T, z, z.right)
          elseif z.right == T.nil
              x = z.left
              RB-TRANSPLANT(T, z, z.left)
          else y = TREE-MINIMUM(z.right)
              y-original-color = y.color
              x = y.right
              if y.p == z
                  x.p = y
              else
                  RB-TRANSPLANT(T, y, y.right)
                  y.right = z.right
                  y.right.p = y
              RB-TRANSPLANT(T, z, y)
              y.left = z.left
              y.left.p = y
              y.color = z.color
          if y-original-color == BLACK
              RB-DELETE-FIXUP(T, x)
  */
  shared_ptr<RBTreeNode> z =
      dynamic_pointer_cast<RBTreeNode>(BinarySearchTree::find_node(value));
  if (!z)
    return;

  shared_ptr<RBTreeNode> y = z;
  Color y_original_color = RBTreeNode::getColor(y);
  shared_ptr<RBTreeNode> x_parent;
  shared_ptr<RBTreeNode> x;
  if (!z->left) {
    x = dynamic_pointer_cast<RBTreeNode>(z->right);
    x_parent = dynamic_pointer_cast<RBTreeNode>(z->parent);
    BinarySearchTree::transplant(z, z->right);
  } else if (!z->right) {
    x = dynamic_pointer_cast<RBTreeNode>(z->left);
    x_parent = dynamic_pointer_cast<RBTreeNode>(z->parent);
    BinarySearchTree::transplant(z, z->left);
  } else {
    y = dynamic_pointer_cast<RBTreeNode>(z->right);
    while (y->left) {
      y = dynamic_pointer_cast<RBTreeNode>(y->left);
    }
    y_original_color = RBTreeNode::getColor(y);
    x = dynamic_pointer_cast<RBTreeNode>(y->right);
    if (y->parent == z) {
      x_parent = y;
    } else {
      x_parent = dynamic_pointer_cast<RBTreeNode>(y->parent);
      BinarySearchTree::transplant(y, x);
      y->right = z->right;
      if (y->right) y->right->parent = y;
    }
    BinarySearchTree::transplant(z, y);
    y->left = z->left;
    if (y->left) y->left->parent = y;
    RBTreeNode::setColor(y, RBTreeNode::getColor(z));
  }

  if (y_original_color == Color::BLACK)
      remove_fixup(x, x_parent);
}

#include <queue>
#include <sstream>

string RedBlackTree::serialize() {
  // Todo: finish this
  // We use "(" + std::to_string(node->value) + ")"  for red nodes
  if (!root)
    return "#";
  std::ostringstream out;
  queue<shared_ptr<TreeNode>> q;
  q.push(root);
  bool isFirst = true;
  int nonNullNode = 1;
  while (nonNullNode) {
    auto node = dynamic_pointer_cast<RBTreeNode>(q.front());
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

void RedBlackTree::deserialize(const string &code) {
  // Todo: finish this
  stringstream ss(code);
  string token;
  ss >> token;
  root = RBTreeNode::makeRBTreeNode(token);
  if (!root)
    return;
  queue<shared_ptr<TreeNode>> q;
  q.push(root);
  while (1) {
    auto node = dynamic_pointer_cast<RBTreeNode>(q.front());
    q.pop();
    if (ss >> token) {
      shared_ptr<RBTreeNode> newNode = RBTreeNode::makeRBTreeNode(token, node);
      node->left = newNode;
      if (newNode)
        q.push(newNode);
    } else {
      break;
    }
    if (ss >> token) {
      shared_ptr<RBTreeNode> newNode = RBTreeNode::makeRBTreeNode(token, node);
      node->right = newNode;
      if (newNode)
        q.push(newNode);
    } else {
      break;
    }
  }
}
