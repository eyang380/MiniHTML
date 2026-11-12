#include "tokenizer.h"
#include <cctype>
#include <cstddef>
#include <string>
#include <vector>
#include <utility>

namespace minihtml {

namespace {

// Read comments from <!-- to -->
std::string readComment(const std::string& html, size_t& pos) {
    size_t start = pos;
    size_t end = html.find("-->", start);

    if (end == std::string::npos) {
        pos = html.size();
    } 
    else {
        pos = end + 3; // Move past the comment
    }

    return html.substr(start, pos - start);
}

// Read tags from < to > while respecting quotation marks
std::string readTag(const std::string& html, size_t& pos) {
    size_t start = pos;
    char quote = '\0';

    // Track whether we're in an attribute value
    while (pos < html.size()) {
        char c = html[pos++];

        if (quote != '\0') {
            if (c == quote) quote = '\0';
        }
        else if (c == '"' || c == '\'') {
            quote = c;
        }
        else if (c == '>') {
            break;
        }
    }

    return html.substr(start, pos - start);
}

// Read text until <
std::string readText(const std::string& html, size_t& pos) {
    size_t start = pos;

    while(pos < html.size() && html[pos] != '<') {
        ++pos;
    }

    return html.substr(start, pos - start);
}

// Classify tags as start/end/self-closing
TokenType classifyTag(const std::string& tag) {
    if (tag.substr(0, 2) == "</") return TokenType::EndTag;
    
    // Make sure the tag has >
    if (tag.empty() || tag.back() != '>') return TokenType::StartTag;

    // Ignore whitespace before closing >
    size_t end = tag.size() - 1;

    while (end > 0 &&
            std::isspace(static_cast<unsigned char>(tag[end - 1]))) {
        --end;
    }

    if (end > 0 && tag[end - 1] == '/') {
        return TokenType::SelfClosingTag;
    }

    return TokenType::StartTag;
}

std::string toLowerCase(std::string str) {
    for (char& c : str) {
        c = static_cast<char>(
            std::tolower(static_cast<unsigned char>(c))
        );
    }

    return str;
}

void parseAttributes(Token& token) {
    const std::string tag = token.value;
    size_t pos = 0;

    if (pos < tag.size() && tag[pos] == '<') ++pos;
    if (pos < tag.size() && tag[pos] == '/') ++pos;

    auto skipWhiteSpace = [&]() {
        while (pos < tag.size() && 
                std::isspace(static_cast<unsigned char>(tag[pos])))
            ++pos;
    };

    // read until ' ', '=', '/', '>'
    auto readSegment = [&]() {
        size_t start = pos;
        while (pos < tag.size() && 
                !std::isspace(static_cast<unsigned char>(tag[pos])) &&
                tag[pos] != '=' && tag[pos] != '>' && tag[pos] != '/')
            ++pos;
        return tag.substr(start, pos - start);
    };

    skipWhiteSpace();
    token.tagName = toLowerCase(readSegment());

    if (token.type == TokenType::EndTag) return;

    while (pos < tag.size()) {
        skipWhiteSpace();
        if (pos >= tag.size() || tag[pos] == '>') break;

        if (tag[pos] == '/') {
            ++pos; 
            continue;
        }

        std::string attrName = readSegment();
        if (attrName.empty()) {
            ++pos;
            continue;
        }

        skipWhiteSpace();
        std::string attrValue;

        if (pos < tag.size() && tag[pos] == '=') {
            ++pos;
            skipWhiteSpace();

            if (pos < tag.size() && (tag[pos] == '"' || tag[pos] == '\'')) {
                char quote = tag[pos++];
                size_t valStart = pos;
                while (pos < tag.size() && tag[pos] != quote) pos++;
                attrValue = tag.substr(valStart, pos - valStart);
                if (pos < tag.size()) ++pos;
            } else {
                size_t valStart = pos;
                while (pos < tag.size() && 
                        !std::isspace(static_cast<unsigned char>(tag[pos])) &&
                        tag[pos] != '>') {
                    ++pos;            
                }
                attrValue = tag.substr(valStart, pos - valStart);
            }
        }

        token.attributes.emplace_back(std::move(attrName), std::move(attrValue));
    }
}
}

std::vector<Token> Tokenizer::tokenize(const std::string& html) {
    std::vector<Token> tokens;
    size_t pos = 0;

    while (pos < html.size()) {
        // Comment
        if (html.compare(pos, 4, "<!--") == 0) { 
            tokens.push_back({
                TokenType::Comment,
                readComment(html, pos)
            });
        }

        // Doctype
        else if (html.compare(pos, 9, "<!DOCTYPE") == 0) {
            tokens.push_back({
                TokenType::Doctype,
                readTag(html, pos)
            });
        }

        // Tag
        else if (html[pos] == '<') {
            std::string tag = readTag(html, pos);
            
            Token token;
            token.type = classifyTag(tag);
            token.value = tag;

            parseAttributes(token);
            tokens.push_back(token);
        }
        
        // Text
        else {
            tokens.push_back({
                TokenType::Text,
                readText(html, pos)
            });
        }
    }

    return tokens;
}

}