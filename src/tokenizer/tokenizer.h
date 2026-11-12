#pragma once

#include <string>
#include <vector>

namespace minihtml {
    enum class TokenType {
        StartTag,
        EndTag,
        SelfClosingTag,
        Text,
        Comment,
        Doctype
    };

    struct Token {
        TokenType type;
        std::string value;
        std::string tagName;
        std::vector<std::pair<std::string, std::string>> attributes;
    };

    class Tokenizer {
    public:
        std::vector<Token> tokenize(const std::string& html);
    };
}
