#pragma once

#include <string>
#include <vector>


enum class NodeType {
    Document,
    Element,
    Text,
    Comment
};

struct Node {
    NodeType type;

    std::string name;
    std::string data;

    Node* parent = nullptr;
    std::vector<Node*> children;
};