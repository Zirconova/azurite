#include "lexer.h"

char Lexer::at()
{
    return source.at(ptr);
}

char Lexer::eat()
{
    col++;
    return source.at(ptr++);
}

char Lexer::peek()
{
    return source.at(ptr + 1);
}

std::vector<Token> Lexer::tokenize(std::string source_)
{
    source = source_;
    ptr = 0;
    line = 1;
    col = 1;
    std::vector<Token> tokens;

    while (ptr < source.length()) {

        switch (at()) {
            // TODO: maybe just put all this operator stuff in a tokenize_operator function
            // Arithmetic operators
            case '+':
            case '-':
            case '*':
            case '/':
            case '%':
            case '^':
                tokens.push_back(Token(TokenType::ArithmeticOperator, {eat()}, line, col));
                break;
            // Comparison operators
            case '>':
            case '<':
            case '=':
                if (peek() == '=') {
                    tokens.push_back(Token(TokenType::ComparisonOperator, {eat(), eat()}, line, col));
                } else if (at() != '=') {
                    tokens.push_back(Token(TokenType::ComparisonOperator, {eat()}, line, col));
                } else {
                    tokens.push_back(Token(TokenType::Equals, {eat()}, line, col));
                }
                break;
            case '!':
                if (peek() == '=') {
                    tokens.push_back(Token(TokenType::ComparisonOperator, {eat(), eat()}, line, col));
            // Logical operators
                } else {
                    tokens.push_back(Token(TokenType::LogicalOperator, {eat()}, line, col));
                }
                break;
            case '&':
            case '|':
                tokens.push_back(Token(TokenType::LogicalOperator, {eat()}, line, col));
                break;
            case '(':
                tokens.push_back(Token(TokenType::OpenParen, {eat()}, line, col));
                break;
            case ')':
                tokens.push_back(Token(TokenType::CloseParen, {eat()}, line, col));
                break;
            case '{':
                tokens.push_back(Token(TokenType::OpenBrace, {eat()}, line, col));
                break;
            case '}':
                tokens.push_back(Token(TokenType::CloseBrace, {eat()}, line, col));
                break;
            case '[':
                tokens.push_back(Token(TokenType::OpenSquare, {eat()}, line, col));
                break;
            case ']':
                tokens.push_back(Token(TokenType::CloseSquare, {eat()}, line, col));
                break;
            case ',':
                tokens.push_back(Token(TokenType::Comma, {eat()}, line, col));
                break;
            case ':':
                tokens.push_back(Token(TokenType::Colon, {eat()}, line, col));
                break;
            case '\n':
                tokens.push_back(Token(TokenType::Endline, {eat()}, line, col));
                line++;
                col = 1;
                break;
            default:
                std::string result = "";
                int begin_line = line;
                int begin_col = col;

                // Handle number token
                if (isdigit(at()) || at() == '.') {
                    int dec_count = 0;
                    while (ptr < source.length() && (isdigit(at()) || at() == '.')) {
                        if (at() == '.') dec_count++;
                        result += eat();
                        if (dec_count > 1) {
                            std::cout << "Invalid number.\n";
                            exit(1);
                        }
                    }

                    tokens.push_back(Token(TokenType::Number, result, begin_line, begin_col));

                // Handle identifier/keyword token
                } else if (isalpha(at())) {
                    while (ptr < source.length() && (isalnum(at()) || at() == '_')) {
                        result += eat();
                    }

                    if (result == "for") {
                        tokens.push_back(Token(TokenType::For, result, begin_line, begin_col));
                    } else if (result == "if") {
                        tokens.push_back(Token(TokenType::If, result, begin_line, begin_col));
                    } else if (result == "Wave") {
                        tokens.push_back(Token(TokenType::Wave, result, begin_line, begin_col));
                    } else if (result == "func") {
                        tokens.push_back(Token(TokenType::Func, result, begin_line, begin_col));
                    } else if (result == "return") {
                        tokens.push_back(Token(TokenType::Return, result, begin_line, begin_col));
                    } else {
                        tokens.push_back(Token(TokenType::Identifier, result, begin_line, begin_col));
                    }
                } else {
                    eat();
                }

                break;
        }
    }

    tokens.push_back(Token(TokenType::Endline, "\n", line, col));
    tokens.push_back(Token(TokenType::EndOfFile, "EOF", line, col));

    return tokens;
}