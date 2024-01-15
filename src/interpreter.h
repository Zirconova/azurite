#include "parser.h"
#include "environment.h"
#include "runtimelib.h"
#include "error.h"

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

    std::shared_ptr<RuntimeVal> get_var(std::string name);
    FunctionDeclaration* get_func(std::string name);
    void exit_scope();

    std::shared_ptr<RuntimeVal> evaluate_stmt(Stmt* node);
    std::shared_ptr<RuntimeVal> evaluate_stmts(Stmts* node);
    void interpret_assignstmt(AssignStmt* node);
    void interpret_functiondeclaration(FunctionDeclaration* node);
    std::shared_ptr<RuntimeVal> evaluate_ifstmt(IfStmt* node);
    std::shared_ptr<RuntimeVal> evaluate_forstmt(ForStmt* node);
    std::shared_ptr<RuntimeVal> evaluate_returnstmt(ReturnStmt* node);
    std::shared_ptr<RuntimeVal> evaluate_expr(Expr* node);
    std::shared_ptr<RuntimeVal> evaluate_identifier(Identifier* node);
    std::shared_ptr<RuntimeVal> evaluate_numericliteral(NumericLiteral* node);
    std::shared_ptr<RuntimeVal> evaluate_callexpr(CallExpr* node);
    // double pointer because in assignexpr you may reassign
    // a list element, which requires changing the pointer in
    // the vector in place
    std::shared_ptr<RuntimeVal>* evaluate_memberexpr(MemberExpr* node);
    std::shared_ptr<RuntimeVal> evaluate_binaryexpr(BinaryExpr* node);
    std::shared_ptr<RuntimeVal> evaluate_unaryexpr(UnaryExpr* node);
    std::shared_ptr<RuntimeVal> evaluate_listdeclaration(ListDeclaration* node);
};