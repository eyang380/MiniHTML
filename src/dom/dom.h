#pragma once

#include <string>
#include <vector>
#include <memory>
#include <utility>

namespace minihtml {

enum class NodeType {
    Document,
    Element,
    Text,
    Comment
};

struct Node {
    NodeType type;
    
    std::string tagName;
    std::string value;

    std::vector<std::pair<std::string, std::string>> attributes;

    std::vector<std::unique_ptr<Node>> children;

    Node* parent = nullptr;
};

}