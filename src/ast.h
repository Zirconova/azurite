#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "token.h"

enum class NodeType
{
    // Expression
    NumericLiteral,
    Identifier,
    UnaryExpr,
    BinaryExpr,
    CallExpr,
    MemberExpr,
    ListDeclaration,
    WaveDeclaration,
    // Statement
    AssignStmt,
    ForStmt,
    IfStmt,
    FunctionDeclaration,
    ReturnStmt,
    Arguments,
    Parameters,
    Stmts,
    Program
};


class Stmt
{
public:
    NodeType type;
    Token begin;

    Stmt(NodeType type, Token begin);
    virtual ~Stmt() {}
};


class Stmts : public Stmt
{
public:
    std::vector<Stmt*> stmts;

    Stmts(std::vector<Stmt*> stmts, Token begin);
    ~Stmts();
};


class Program : public Stmt
{
public:
    Stmts* body;

    Program(Stmts* body, Token begin);
    ~Program();
};


class Expr : public Stmt
{
public:
    Expr(NodeType type, Token begin);
    virtual ~Expr() {}
};


class BinaryExpr : public Expr
{
public:
    Expr* lhs;
    Expr* rhs;
    Token op;

    BinaryExpr(Expr* lhs, Expr* rhs, Token op, Token begin);
    ~BinaryExpr();
};


class UnaryExpr : public Expr
{
public:
    Expr* operand;
    Token op;

    UnaryExpr(Expr* operand, Token op, Token begin);
    ~UnaryExpr();
};


class NumericLiteral : public Expr
{
public:
    double value;

    NumericLiteral(double value, Token begin);
    ~NumericLiteral() {}
};


class Identifier : public Expr
{
public:
    std::string name;

    Identifier(std::string name, Token begin);
    ~Identifier() {}
};


class Arguments : public Stmt
{
public:
    std::vector<Expr*> arguments;

    Arguments(std::vector<Expr*> arguments, Token begin);
    ~Arguments();
};


class Parameters : public Stmt
{
public:
    std::vector<Identifier*> parameters;

    Parameters(std::vector<Identifier*> parameters, Token begin);
    ~Parameters();
};


class CallExpr : public Expr
{
public:
    Identifier* callee;
    Arguments* arguments;

    CallExpr(Identifier* callee, Arguments* arguments, Token begin);
    ~CallExpr();
};


class FunctionDeclaration : public Stmt
{
public:
    Identifier* name;
    Parameters* params;
    Stmts* body;
    int ref_count;

    FunctionDeclaration(Identifier* name, Parameters* params, Stmts* body, Token begin);
    ~FunctionDeclaration();
};


class ReturnStmt : public Stmt
{
public:
    Expr* return_val;

    ReturnStmt(Expr* return_val, Token begin);
    ~ReturnStmt();
};


class MemberExpr : public Expr
{
public:
    Expr* object;
    Expr* index;

    MemberExpr(Expr* object, Expr* index, Token begin);
    ~MemberExpr();
};


class ListDeclaration : public Expr
{
public:
    std::vector<Expr*> elements;

    ListDeclaration(std::vector<Expr*> elements, Token begin);
    ~ListDeclaration();
};


class ForStmt : public Stmt
{
public:
    Identifier* iterator;
    Expr* start;
    Expr* end;
    Stmts* body;

    ForStmt(Identifier* iterator, Expr* start, Expr* end, Stmts* body, Token begin);
    ~ForStmt();
};


class IfStmt : public Stmt
{
public:
    Expr* condition;
    Stmts* body;

    IfStmt(Expr* condition, Stmts* body, Token begin);
    ~IfStmt();
};


class AssignStmt : public Stmt
{
public:
    Expr* lhs;
    Expr* rhs;

    AssignStmt(Expr* lhs, Expr* rhs, Token begin);
    ~AssignStmt();
};


class WaveDeclaration : public Expr
{
public:
    FunctionDeclaration* wave_func;
    FunctionDeclaration* freq_func;
    FunctionDeclaration* phase_func;
    FunctionDeclaration* vol_func;
    FunctionDeclaration* pan_func;

    WaveDeclaration(
        FunctionDeclaration* wave_func,
        FunctionDeclaration* freq_func,
        FunctionDeclaration* phase_func,
        FunctionDeclaration* vol_func,
        FunctionDeclaration* pan_func,
        Token begin
    );
    ~WaveDeclaration();
};


void printAST(Stmt* node, int indent = 0, bool in_list = false);

void formatNode(std::string name, Stmt* node, int indent);

void printIndent(int depth);

