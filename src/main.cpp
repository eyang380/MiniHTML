#include "tokenizer/tokenizer.h"

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
    std::string teststring = R"(
<!DOCTYPE html>
<!-- Page comment -->

<html>
    <head>
        <title>My Website</title>
    </head>
    <body>
        <!-- This is a comment -->
        <h1 class="title">Hello World!</h1>
        <p>This is a paragraph.</p>
        <br/>
    </body>
</html>

<div>
    <!-- Another comment -->
    <p>Visible text</p>
</div>

<body>
    <p>First line<br>Second line</p>
    <img src="photo.jpg">
    <input type="text" disabled>
</body>

<div class="container" id="main">
    <a href="https://example.com" title="A > B">Click here</a>
    <img src="image.png" alt="A picture"/>
</div>

<pre>
    First line
        Second line

    Fourth line
</pre>

<p>Hello<strong>very</strong>important<em>!</em></p>
<p title='hello > world'>Text</p>
)";

    Tokenizer tokenizer;

    std::vector<Token> tokens = tokenizer.tokenize(teststring);
    
    for (const Token& token : tokens) {
        std::cout << "[" << tokenTypeName(token.type) << "] ";

        if (token.type == TokenType::Text ||
            token.type == TokenType::Comment ||
            token.type == TokenType::Doctype) {

            std::cout << escapeWhitespace(token.value) << '\n';
            continue;
        }

        std::cout << "Tag: " << token.tagName << '\n';

        for (const auto& [name, value] : token.attributes) {
            std::cout << "    Attribute: " << name
                    << " = \"" << value << "\"\n";
        }
    }

    return 0;
}