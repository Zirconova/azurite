#pragma once

#include <iostream>
#include <vector>

#include "token.h"

class Lexer
{
public:
    Lexer() {}
    ~Lexer() {}

    char at();
    char eat();
    char peek();
    std::vector<Token> tokenize(std::string source);

private:
    std::string source;
    int ptr;
    int line;
    int col;
};