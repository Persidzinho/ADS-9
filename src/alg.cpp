// Copyright 2022 NNTU-CS
#include <algorithm>
#include <vector>

#include "tree.h"

PMTree::PMTree(const std::vector<char>& symbols) : root(nullptr), total(0) {
    buildTree(symbols);
}

PMTree::~PMTree() {
    if (root) destroySubtree(root);
}

void PMTree::buildTree(const std::vector<char>& symbols) {
    if (symbols.empty()) {
        root = new Node('\0');
        root->count = 1;
        total = 1;
        return;
    }
    std::vector<char> sorted = symbols;
    std::sort(sorted.begin(), sorted.end());
    auto last = std::unique(sorted.begin(), sorted.end());
    sorted.erase(last, sorted.end());

    root = new Node('\0');
    root->count = 0;
    for (char ch : sorted) {
        Node* child = new Node(ch);
        root->children.push_back(child);
        std::vector<char> remaining;
        for (char c : sorted) {
            if (c != ch) remaining.push_back(c);
        }
        buildSubtree(child, remaining);
        root->count += child->count;
    }
    total = root->count;
}

PMTree::Node* PMTree::buildSubtree(Node* parent,
                                   const std::vector<char>& available) {
    if (available.empty()) {
        parent->count = 1;
        return parent;
    }
    parent->count = 0;
    for (char ch : available) {
        Node* child = new Node(ch);
        parent->children.push_back(child);
        std::vector<char> remaining;
        for (char c : available) {
            if (c != ch) remaining.push_back(c);
        }
        buildSubtree(child, remaining);
        parent->count += child->count;
    }
    return parent;
}

void PMTree::destroySubtree(Node* node) {
    for (Node* child : node->children) {
        destroySubtree(child);
    }
    delete node;
}

static void dfsCollect(const PMTree::Node* node,
                       std::vector<char>& current,
                       std::vector<std::vector<char>>& output) {
    for (const auto* child : node->children) {
        current.push_back(child->value);
        if (child->children.empty()) {
            output.push_back(current);
        } else {
            dfsCollect(child, current, output);
        }
        current.pop_back();
    }
}

std::vector<std::vector<char>> getAllPerms(const PMTree& tree) {
    std::vector<std::vector<char>> result;
    if (!tree.getRoot()) return result;
    std::vector<char> path;
    dfsCollect(tree.getRoot(), path, result);
    return result;
}

static bool dfsFind(const PMTree::Node* node,
                    std::vector<char>& path,
                    int& counter,
                    int target,
                    std::vector<char>& out) {
    for (const auto* child : node->children) {
        path.push_back(child->value);
        if (child->children.empty()) {
            ++counter;
            if (counter == target) {
                out = path;
                path.pop_back();
                return true;
            }
        } else {
            if (dfsFind(child, path, counter, target, out)) {
                path.pop_back();
                return true;
            }
        }
        path.pop_back();
    }
    return false;
}

std::vector<char> getPerm1(const PMTree& tree, int num) {
    std::vector<char> result;
    if (num < 1 || num > tree.totalPerms()) return result;
    std::vector<char> path;
    int counter = 0;
    dfsFind(tree.getRoot(), path, counter, num, result);
    return result;
}

std::vector<char> getPerm2(const PMTree& tree, int num) {
    std::vector<char> result;
    if (num < 1 || num > tree.totalPerms()) return result;
    const auto* current = tree.getRoot();
    while (current && num > 0) {
        for (const auto* child : current->children) {
            if (num <= child->count) {
                result.push_back(child->value);
                current = child;
                break;
            } else {
                num -= child->count;
            }
        }
        if (current->children.empty()) break;
    }
    return result;
}
