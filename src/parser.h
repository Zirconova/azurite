#pragma once

#include "ast.h"
#include "lexer.h"

class Parser
{
public:

    Parser() {}
    ~Parser() {}
    
    Program* parse(std::string source);

private:
    std::vector<Token> tokens;
    int ptr;
    Lexer lexer;

    // Token list methods
    Token at();
    Token eat();
    Token expect(TokenType type, std::string msg);
    void syntax_error(std::string msg);
    void skip_whitespace();
    Token peek();

    // Parsing methods
    Stmts* parse_stmts();
    Stmts* parse_block();

    // Fundamental statements
    Stmt* parse_stmt();
    AssignStmt* parse_assignstmt();
    IfStmt* parse_ifstmt();
    ForStmt* parse_forstmt();
    ReturnStmt* parse_returnstmt();
    FunctionDeclaration* parse_functiondeclaration();
    Parameters* parse_parameters();
    Arguments* parse_arguments();

    // Expressions
    Expr* parse_expr();
    Expr* parse_or();
    Expr* parse_and();
    Expr* parse_comp();
    Expr* parse_sum();
    Expr* parse_term();
    Expr* parse_factor();
    Expr* parse_unaryexpr();
    Expr* parse_primaryexpr();
    Expr* parse_memberexpr();
    CallExpr* parse_callexpr();
    ListDeclaration* parse_listdeclaration();
    WaveDeclaration* parse_wavedeclaration();
};