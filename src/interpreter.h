#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <memory>
#include <unordered_map>

#include "ast.h"
#include "parser.h"
#include "runtimeval.h"
#include "environment.h"
#include "runtimelib.h"
#include "error.h"
#include "wavewriter.h"
#include "exprreduction.h"

typedef std::shared_ptr<RuntimeVal> RuntimeValPtr;


class Interpreter
{
public:

    Interpreter();
    ~Interpreter();

    void interpret(std::string source);

private:
    Parser parser;
    Program* program;
    Environment* global_scope;
    std::vector<Environment*> scopes;

    std::unordered_map<std::string, WaveBuffer*> wave_buffers;

    RuntimeValPtr get_var(std::string name);
    FunctionDeclaration* get_func(std::string name);
    void create_var(std::string name, RuntimeValPtr value);
    void create_func(std::string name, FunctionDeclaration* func);
    void new_scope();
    void exit_scope();

    RuntimeValPtr evaluate_stmt(Stmt* node);
    RuntimeValPtr evaluate_stmts(Stmts* node);
    void interpret_assignstmt(AssignStmt* node);
    void interpret_functiondeclaration(FunctionDeclaration* node);
    RuntimeValPtr evaluate_ifstmt(IfStmt* node);
    RuntimeValPtr evaluate_forstmt(ForStmt* node);
    RuntimeValPtr evaluate_returnstmt(ReturnStmt* node);
    RuntimeValPtr evaluate_expr(Expr* node);
    RuntimeValPtr evaluate_runtimevalpointernode(RuntimeValPointerNode* node);
    RuntimeValPtr evaluate_numberpointernode(NumberPointerNode* node);
    RuntimeValPtr evaluate_identifier(Identifier* node);
    RuntimeValPtr evaluate_stringliteral(StringLiteral* node);
    RuntimeValPtr evaluate_numericliteral(NumericLiteral* node);
    RuntimeValPtr evaluate_callexpr(CallExpr* node);
    // double pointer because in assignexpr you may reassign
    // a list element, which requires changing the pointer in
    // the vector in place
    RuntimeValPtr* evaluate_memberexpr(MemberExpr* node);
    RuntimeValPtr evaluate_binaryexpr(BinaryExpr* node);
    RuntimeValPtr evaluate_unaryexpr(UnaryExpr* node);
    RuntimeValPtr evaluate_listdeclaration(ListDeclaration* node);
    RuntimeValPtr evaluate_wavedeclaration(WaveDeclaration* node);

    void simplify_wave(std::shared_ptr<Wave> wave);
    void desimplify_wave(std::shared_ptr<Wave> wave);
    Expr* simplify_expr(Expr* node, std::shared_ptr<Wave> wave);

    RuntimeValPtr write_wave(std::vector<RuntimeValPtr> args);
    double get_sample_and_advance(std::shared_ptr<Wave> wave);
};
