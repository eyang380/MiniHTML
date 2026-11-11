#include "tokenizer.h"

#include <iostream>
#include <string>

using namespace minihtml;

std::string tokenTypeName(TokenType type) {
    switch (type) {
        case TokenType::StartTag: return "StartTag";
        case TokenType::EndTag: return "EndTag";
        case TokenType::SelfClosingTag: return "SelfClosingTag";
        case TokenType::Text: return "Text";
        case TokenType::Comment: return "Comment";
        case TokenType::Doctype: return "Doctype";
    }

    return "Unknown";
}

std::string escapeWhitespace(const std::string& text) {
    std::string result;

    for (char c : text) {
        switch (c) {
            case '\n':
                result += "\\n";
                break;
            case '\t':
                result += "\\t";
                break;
            case '\r':
                result += "\\r";
                break;
            default:
                result += c;
        }
    }

    return result;
}

int main() {
    std::string html = R"(
<p>Hello<strong>very</strong>important<em>!</em></p>
<p title='hello > world'>Text</p>
)";

    Tokenizer tokenizer;

    std::vector<Token> tokens = tokenizer.tokenize(html);
    
    for (const Token& token : tokens) {
        std::cout <<"[" << tokenTypeName(token.type) << "] ";

        if (token.type == TokenType::Text) {
            std::cout << "\"" << escapeWhitespace(token.value) << "\"";
        } else {
            std::cout << token.value;
        }
        
        std::cout << '\n';
    }

    return 0;
}