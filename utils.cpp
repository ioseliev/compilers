#include "include/utils.hpp"

#include <string>
#include <algorithm> 

std::string trim(const std::string& s) {
    const auto start = s.find_first_not_of(" \t");
    const auto end = s.find_last_not_of(" \t");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
};

bool isVariable(const std::string& token) {
    
    return !(token == "+" || token == "-" || token == "*" || token == "/" || token == "if" || token == "goto" ||  token == "<" || token == ">" || token == ">=" || token == "<=" || token == "==" ||
                std::all_of(token.begin(), token.end(), ::isdigit));
};

bool isComparator(const std::string& t) {
    auto eq_pos = t.find('=');
    if (eq_pos != std::string::npos) {
        if ((eq_pos > 0 && (t[eq_pos - 1] == '<' || t[eq_pos - 1] == '>')) ||
            (eq_pos + 1 < t.size() && t[eq_pos + 1] == '=')) {
            return true;
        }
    }
    return false;
}

static inline bool isOperator(const unsigned char c) {
    switch (c) {
        case '+':
        case '-':
        case '*':
        case '/': {
            return true;
        }
        default: {
            return false;
        }
    }
}

bool isExpression(const std::string& s) {
    for (const char c : s) {
        if (isOperator(c)) {
            return true;
        }
    }
    return false;
}