// Copyright 2022 NNTU-CS
#ifndef INCLUDE_TREE_H_
#define INCLUDE_TREE_H_

#include <vector>

class PMTree {
 public:
    explicit PMTree(const std::vector<char>& symbols);
    ~PMTree();

    const struct Node* getRoot() const { return root; }
    int totalPerms() const { return total; }

 private:
    struct Node {
        char value;
        std::vector<Node*> children;
        int count;
        explicit Node(char c) : value(c), count(0) {}
    };

    Node* root;
    int total;

    void buildTree(const std::vector<char>& symbols);
    Node* buildSubtree(Node* parent, const std::vector<char>& available);
    void destroySubtree(Node* node);
};

std::vector<std::vector<char>> getAllPerms(const PMTree& tree);
std::vector<char> getPerm1(const PMTree& tree, int num);
std::vector<char> getPerm2(const PMTree& tree, int num);

#endif  // INCLUDE_TREE_H_
