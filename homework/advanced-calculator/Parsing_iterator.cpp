#include "Parsing_iterator.hpp"
#include <cwctype>

Parsing_Iterator& Parsing_Iterator::operator=(const char& letter) {
    if (std::iswspace(letter)) {
        return (*this);
    }

    if (letter == SeparatorFormat::negative) {
        if (state_ == operation) {
            num_.push_back({});
            num_.back() += letter;
            state_ = number;
        } else {
            op_.push_back(letter);
            state_ = operation;
        }
        return (*this);
    }

    if (!std::isdigit(letter) && letter != SeparatorFormat::legalOnce) {
        op_.push_back(letter);
        state_ = operation;
        return (*this);
    }

    if (state_ == operation) {
        num_.push_back({});
        state_ = number;
    }
    num_.back() += letter;
    return (*this);
}

Result Parsing_Iterator::getResult() {
    return Result(num_, op_);
}
