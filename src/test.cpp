#include <cassert>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>

#include "BinarySearchTree.hpp"
#include "RedBlackTree.hpp"
using namespace std;

void testInt() {
  {
    BinarySearchTree<int> tree;
    tree.insert(1);
    assert(tree.inorder() == "1");
    tree.insert(2);
    assert(tree.inorder() == "1 2");
    tree.insert(3);
    assert(tree.inorder() == "1 2 3");
    tree.insert(4);
    assert(tree.inorder() == "1 2 3 4");
    tree.insert(5);
    assert(tree.inorder() == "1 2 3 4 5");
    tree.insert(6);
    assert(tree.inorder() == "1 2 3 4 5 6");
    tree.insert(7);
    assert(tree.inorder() == "1 2 3 4 5 6 7");
    tree.insert(8);
    assert(tree.inorder() == "1 2 3 4 5 6 7 8");
    tree.insert(9);
    assert(tree.inorder() == "1 2 3 4 5 6 7 8 9");
    std::cout << "test integer 1 passed!\n";
  }

  {
    BinarySearchTree<int> tree;
    for (int i = 0; i < 20; i++)
      tree.insert(i);
    assert(tree.inorder() == "0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19");
  }
}

void testString() {
  {
    BinarySearchTree<string> tree;
    tree.insert("aaa");
    tree.insert("bbb");
    tree.insert("ccc");
    assert(tree.inorder() == "aaa bbb ccc");

    cout << "test string 1 passed\n";
  }
}

void testRedBlackTrees() 
{
    {
        RedBlackTree<int> tree;
        tree.insert(1);
        assert(tree.inorder() == "1");
        tree.insert(2);
        assert(tree.inorder() == "1 2");
        tree.insert(3);
        assert(tree.inorder() == "1 2 3");
        tree.insert(4);
        assert(tree.inorder() == "1 2 3 4");
        tree.insert(5);
        assert(tree.inorder() == "1 2 3 4 5");
        tree.insert(6);
        assert(tree.inorder() == "1 2 3 4 5 6");
        tree.insert(7);
        assert(tree.inorder() == "1 2 3 4 5 6 7");
        tree.insert(8);
        assert(tree.inorder() == "1 2 3 4 5 6 7 8");
        tree.insert(9);
        assert(tree.inorder() == "1 2 3 4 5 6 7 8 9");
        std::cout << "testRedBlackTrees 1 passed!\n";
    }

    
    {
        RedBlackTree<string> tree;
        tree.insert("aaa");
        assert(tree.inorder() == "aaa");
        tree.insert("bbb");
        assert(tree.inorder() == "aaa bbb");
        tree.insert("ccc");
        assert(tree.inorder() == "aaa bbb ccc");
        std::cout << "testRedBlackTrees 2 passed!\n";
    }

    /*
    {
        RedBlackTree<int> tree;
        tree.deserialize("4 (2) (6) 1 3 5 8           # (7) (9)");
        tree.insert(10);
        assert(tree.inorder() == "4 2 6 1 3 5 (8) # # # # # # 7 9 # # # (10)");
        tree.insert(11);
        assert(tree.inorder() == "4 2 6 1 3 5 (8) # # # # # # 7 10 # # (9) (11)");
        tree.insert(12);
        assert(tree.inorder() == "4 2 8 1 3 (6) (10) # # # # 5 7 9 11 # # # # # # # (12)");
        std::cout << "testRedBlackTrees 3 passed!\n";
    }

    {
        RedBlackTree<int> tree;
        tree.deserialize("4 (2) (6) 1 3 5 8 # # # # # # (7) (9)");
        assert(tree.inorder() == "4 (2) (6) 1 3 5 8 # # # # # # (7) (9)");
        tree.remove(1);
        assert(tree.inorder() == "4 2 (6) # (3) 5 8 # # # # (7) (9)");
        tree.remove(2);
        assert(tree.inorder() == "4 3 (6) # # 5 8 # # (7) (9)");
        tree.remove(3);
        assert(tree.inorder() == "6 4 8 # (5) (7) (9)");
        tree.remove(4);
        assert(tree.inorder() == "6 5 8 # # (7) (9)");
        tree.remove(5);
        assert(tree.inorder() == "8 6 9 # (7)");
        tree.remove(6);
        assert(tree.inorder() == "8 7 9");
        tree.remove(7);
        assert(tree.inorder() == "8 # (9)");
        tree.remove(8);
        assert(tree.inorder() == "9");
        std::cout << "testRedBlackTrees 4 passed!\n";
    }

    {
        RedBlackTree<int> tree;
        tree.deserialize("4 (2) (6) 1 3 5 8 # # # # # # (7) (9)");
        tree.remove(4);
        assert(tree.inorder() == "5 (2) (8) 1 3 6 9 # # # # # (7)");
        tree.insert(4); 
        assert(tree.inorder() == "5 (2) (8) 1 3 6 9 # # # (4) # (7)");
        tree.remove(5); 
        assert(tree.inorder() == "6 (2) (8) 1 3 7 9 # # # (4)");
        tree.insert(5); 
        assert(tree.inorder() == "6 (2) (8) 1 4 7 9 # # (3) (5)");
        tree.remove(6); 
        assert(tree.inorder() == "7 (2) 8 1 4 # (9) # # (3) (5)");
        tree.insert(6); 
        assert(tree.inorder() == "4 (2) (7) 1 3 5 8 # # # # # (6) # (9)");
        std::cout << "testRedBlackTrees 5 passed!\n";
    }

    {
        RedBlackTree<int> tree1;
        tree1.insert(7);
        tree1.insert(3);
        tree1.insert(18);  
        tree1.insert(10);  
        tree1.insert(22);  
        tree1.insert(8); 
        tree1.insert(11);
        tree1.insert(26);
        tree1.insert(12);
        tree1.remove(10);
        tree1.remove(11);

        RedBlackTree<int> tree2;
        tree2.deserialize("10 (7) (18) 3 8 11 22 # # # # # (12) # (26)");
        tree2.remove(10);
        tree2.remove(11);
    
        assert(tree1.inorder() == tree2.inorder());
        std::cout << "testRedBlackTrees 6 passed!\n";
    }
    */
}

int main() {
  testInt();
  testString();
  testRedBlackTrees();

  return 0;
}