#include "tokenizer/tokenizer.h"
#include "dom/dom.h"
#include "parser/parser.h"
#include <memory>
#include <vector>
#include <cctype>
#include <string>

namespace minihtml {

namespace {

bool equalsIgnoreCase (const std::string& a, const std::string& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (std::tolower(static_cast<unsigned char>(a[i])) !=
            std::tolower(static_cast<unsigned char>(b[i]))) {
            return false;
        }
    }

    return true;
}

bool isVoid (const std::string& tagName) {
    static const std::vector<std::string> voidElements = {
        "area", "base", "br", "col", "embed",
        "hr", "img", "input", "link", "meta",
        "source", "track", "wbr"
    };

    for (const auto& tag : voidElements) {
        if (equalsIgnoreCase(tagName, tag)) {
            return true;
        }
    }

    return false;
}


std::unique_ptr<Node> createNode (
    NodeType type, 
    const Token& token,
    Node* parent
) {
    auto node = std::make_unique<Node>();

    node->type = type;
    node->parent = parent;

    if (type == NodeType::Element) {
        node->tagName = token.tagName;
        node->attributes = token.attributes;
    } else {
        node->value = token.value;
    }

    return node;
}

}

std::unique_ptr<Node> Parser::parse(const std::vector<Token>& tokens) {
    auto root = std::make_unique<Node>();
    root->type = NodeType::Document;

    std::vector<Node*> stack;
    stack.push_back(root.get());

    for (const auto& token : tokens) {
        Node* currentParent = stack.back();

        switch(token.type){
            case TokenType::StartTag:
            case TokenType::SelfClosingTag: {
                auto newNode = createNode(NodeType::Element, token, currentParent);

                Node* rawPtr = newNode.get();
                currentParent->children.push_back(std::move(newNode));
                if (!isVoid(token.tagName)) stack.push_back(rawPtr);
                break;
            }
            case TokenType::Text: {
                auto newNode = createNode(NodeType::Text, token, currentParent);

                currentParent->children.push_back(std::move(newNode));
                break;
            }
            case TokenType::Comment: {
                auto newNode = createNode(NodeType::Comment, token, currentParent);

                currentParent->children.push_back(std::move(newNode));
                break;
            }
            case TokenType::EndTag: {
                for (size_t i = stack.size(); i > 1; --i) {
                    if (equalsIgnoreCase(stack[i-1]->tagName, token.tagName)) {
                        stack.resize(i - 1);
                        break;
                    }
                }
                break;
            }
            // Implement Later?
            case TokenType::Doctype:
                break;
        }
    }

    return root;
}

}