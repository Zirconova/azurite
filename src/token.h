#pragma once

#include <string>

enum class TokenType
{
    Identifier,
    Number,
    ArithmeticOperator,
    ComparisonOperator,
    LogicalOperator,
    OpenParen,
    CloseParen,
    OpenBrace,
    CloseBrace,
    OpenSquare,
    CloseSquare,
    Equals,
    Endline,
    Comma,
    Colon,
    EndOfFile,
    // Keywords
    For,
    If,
    Func,
    Wave,
    Write,
    Return
};


class Token
{
public:
    TokenType type;
    std::string value;

    int line;
    int col;

    Token(TokenType type, std::string value, int line, int col)
        : type(type), value(value), line(line), col(col) {}
};