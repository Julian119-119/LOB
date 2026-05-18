#include <iostream>
#include <queue>
#include <sstream>
#include <cassert>

#include "BinarySearchTree.h"
#include "RedBlackTree.h"



void testBinarySearchTrees()
{
    {
        BinarySearchTree tree;
        tree.insert(1);
        assert(tree.serialize() == "1");
        tree.insert(2);
        assert(tree.serialize() == "1 # 2");
        tree.insert(3);
        assert(tree.serialize() == "1 # 2 # 3");
        tree.insert(4);
        assert(tree.serialize() == "1 # 2 # 3 # 4");
        tree.insert(5);
        assert(tree.serialize() == "1 # 2 # 3 # 4 # 5");
        tree.insert(6);
        assert(tree.serialize() == "1 # 2 # 3 # 4 # 5 # 6");
        tree.insert(7);
        assert(tree.serialize() == "1 # 2 # 3 # 4 # 5 # 6 # 7");
        tree.insert(8);
        assert(tree.serialize() == "1 # 2 # 3 # 4 # 5 # 6 # 7 # 8");
        tree.insert(9);
        assert(tree.serialize() == "1 # 2 # 3 # 4 # 5 # 6 # 7 # 8 # 9");
        std::cout << "testBinarySearchTrees 1 passed!\n";
    }

    {
        BinarySearchTree tree1;
        tree1.insert(50);
        tree1.insert(30);
        tree1.insert(70);
        tree1.insert(20);
        tree1.insert(40);
        assert(tree1.serialize() == "50 30 70 20 40");
        tree1.remove(50);
        assert(tree1.serialize() == "70 30 # 20 40");
        tree1.remove(70);
        assert(tree1.serialize() == "30 20 40");

        BinarySearchTree tree2;
        tree2.deserialize("50 30 70 20 40");
        tree2.remove(50);
        assert(tree2.serialize() == "70 30 # 20 40");
        tree2.remove(70);
        assert(tree2.serialize() == tree1.serialize());
        std::cout << "testBinarySearchTrees 2 passed!\n";
    }

    {
        BinarySearchTree tree;
        tree.insert(1);  
        tree.insert(2);
        tree.insert(3);
        assert(tree.serialize() == "1 # 2 # 3");
        tree.rotate_left(tree.find_node(1));  
        assert(tree.serialize() == "2 1 3");
        tree.rotate_right(tree.find_node(2));
        assert(tree.serialize() == "1 # 2 # 3");
        std::cout << "testBinarySearchTrees 3 passed!\n";
    }

    {
        BinarySearchTree tree;
        tree.deserialize("50 30 70 20 40");
        tree.rotate_left(tree.find_node(50));
        assert(tree.serialize() == "70 50 # 30 # 20 40");
        tree.rotate_right(tree.find_node(70));
        assert(tree.serialize() == "50 30 70 20 40");
        tree.rotate_left(tree.find_node(50));
        assert(tree.serialize() == "70 50 # 30 # 20 40");
        tree.rotate_right(tree.find_node(50));
        assert(tree.serialize() == "70 30 # 20 50 # # 40");
        std::cout << "testBinarySearchTrees 4 passed!\n";
    }
}


void testRedBlackTrees() 
{
    {
        RedBlackTree tree;
        tree.insert(1);
        assert(tree.serialize() == "1");
        tree.insert(2);
        assert(tree.serialize() == "1 # (2)");
        tree.insert(3);
        assert(tree.serialize() == "2 (1) (3)");
        tree.insert(4);
        assert(tree.serialize() == "2 1 3 # # # (4)");
        tree.insert(5);
        assert(tree.serialize() == "2 1 4 # # (3) (5)");
        tree.insert(6);
        assert(tree.serialize() == "2 1 (4) # # 3 5 # # # (6)");
        tree.insert(7);
        assert(tree.serialize() == "2 1 (4) # # 3 6 # # (5) (7)");
        tree.insert(8);
        assert(tree.serialize() == "4 (2) (6) 1 3 5 7 # # # # # # # (8)");
        tree.insert(9);
        assert(tree.serialize() == "4 (2) (6) 1 3 5 8 # # # # # # (7) (9)");
        std::cout << "testRedBlackTrees 1 passed!\n";
    }

    {
        RedBlackTree tree;
        tree.deserialize("2 (1) (3)");
        tree.insert(4);
        assert(tree.serialize() == "2 1 3 # # # (4)");
        tree.insert(5);
        assert(tree.serialize() == "2 1 4 # # (3) (5)");
        tree.insert(6);
        assert(tree.serialize() == "2 1 (4) # # 3 5 # # # (6)");
        std::cout << "testRedBlackTrees 2 passed!\n";
    }

    {
        RedBlackTree tree;
        tree.deserialize("4 (2) (6) 1 3 5 8 # # # # # # (7) (9)");
        tree.insert(10);
        assert(tree.serialize() == "4 2 6 1 3 5 (8) # # # # # # 7 9 # # # (10)");
        tree.insert(11);
        assert(tree.serialize() == "4 2 6 1 3 5 (8) # # # # # # 7 10 # # (9) (11)");
        tree.insert(12);
        assert(tree.serialize() == "4 2 8 1 3 (6) (10) # # # # 5 7 9 11 # # # # # # # (12)");
        std::cout << "testRedBlackTrees 3 passed!\n";
    }

    {
        RedBlackTree tree;
        tree.deserialize("4 (2) (6) 1 3 5 8 # # # # # # (7) (9)");
        assert(tree.serialize() == "4 (2) (6) 1 3 5 8 # # # # # # (7) (9)");
        tree.remove(1);
        assert(tree.serialize() == "4 2 (6) # (3) 5 8 # # # # (7) (9)");
        tree.remove(2);
        assert(tree.serialize() == "4 3 (6) # # 5 8 # # (7) (9)");
        tree.remove(3);
        assert(tree.serialize() == "6 4 8 # (5) (7) (9)");
        tree.remove(4);
        assert(tree.serialize() == "6 5 8 # # (7) (9)");
        tree.remove(5);
        assert(tree.serialize() == "8 6 9 # (7)");
        tree.remove(6);
        assert(tree.serialize() == "8 7 9");
        tree.remove(7);
        assert(tree.serialize() == "8 # (9)");
        tree.remove(8);
        assert(tree.serialize() == "9");
        std::cout << "testRedBlackTrees 4 passed!\n";
    }

    {
        RedBlackTree tree;
        tree.deserialize("4 (2) (6) 1 3 5 8 # # # # # # (7) (9)");
        tree.remove(4);
        assert(tree.serialize() == "5 (2) (8) 1 3 6 9 # # # # # (7)");
        tree.insert(4); 
        assert(tree.serialize() == "5 (2) (8) 1 3 6 9 # # # (4) # (7)");
        tree.remove(5); 
        assert(tree.serialize() == "6 (2) (8) 1 3 7 9 # # # (4)");
        tree.insert(5); 
        assert(tree.serialize() == "6 (2) (8) 1 4 7 9 # # (3) (5)");
        tree.remove(6); 
        assert(tree.serialize() == "7 (2) 8 1 4 # (9) # # (3) (5)");
        tree.insert(6); 
        assert(tree.serialize() == "4 (2) (7) 1 3 5 8 # # # # # (6) # (9)");
        std::cout << "testRedBlackTrees 5 passed!\n";
    }

    {
        RedBlackTree tree1;
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

        RedBlackTree tree2;
        tree2.deserialize("10 (7) (18) 3 8 11 22 # # # # # (12) # (26)");
        tree2.remove(10);
        tree2.remove(11);
    
        assert(tree1.serialize() == tree2.serialize());
        std::cout << "testRedBlackTrees 6 passed!\n";
    }
}


int main() 
{
    testBinarySearchTrees();
    testRedBlackTrees();

    return 0;
}