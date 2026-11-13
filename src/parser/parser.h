#pragma once

#include "tokenizer/tokenizer.h"
#include "dom/dom.h"

#include <memory>
#include <vector>

namespace minihtml {

class Parser {
public:
    std::unique_ptr<Node> parse(const std::vector<Token>& tokens);
};

}