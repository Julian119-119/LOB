#ifndef __REDBLACKTREE_H__
#define __REDBLACKTREE_H__

#include "BinarySearchTree.h"

enum class Color {RED, BLACK};

struct RBTreeNode : TreeNode {
    // Todo: implement this in your way
    Color color;
    RBTreeNode(int v): color(Color::RED), TreeNode(v) {}
    RBTreeNode(int v, Color c): color(c), TreeNode(v) {}
    RBTreeNode(int v, Color c, shared_ptr<TreeNode> p): color(c), TreeNode(v, p) {}
    ~RBTreeNode() = default;
    
    string toString();
    static shared_ptr<RBTreeNode> makeRBTreeNode(string& s, shared_ptr<TreeNode> p = nullptr);
    static Color getColor(shared_ptr<TreeNode> node);
    static void setColor(shared_ptr<TreeNode> node, Color newcolor);
};


class RedBlackTree : public BinarySearchTree {
public:
    RedBlackTree() : BinarySearchTree() {};
    void insert(int value) override;
    void remove(int value) override;
    std::string serialize() override;
    void deserialize(const std::string& code) override;

private:
    void insert_fixup(shared_ptr<RBTreeNode> z);
    void remove_fixup(shared_ptr<RBTreeNode> x, shared_ptr<RBTreeNode> parent);
};

#endif