#include "ast.h"

Stmt::Stmt(NodeType type, Token begin)
    : type(type), begin(begin) {}

Stmts::Stmts(std::vector<Stmt*> stmts, Token begin)
    : Stmt(NodeType::Stmts, begin), stmts(stmts) {}
Stmts::~Stmts()
{
    for (Stmt* stmt : stmts) {
        delete stmt;
    }
}

Program::Program(Stmts* body, Token begin)
    : Stmt(NodeType::Program, begin), body(body) {}
Program::~Program()
{
    delete body;
}

ForStmt::ForStmt(Identifier* iterator, Expr* start, Expr* end, Stmts* body, Token begin)
    : Stmt(NodeType::ForStmt, begin), iterator(iterator), start(start), end(end), body(body) {}
ForStmt::~ForStmt()
{
    delete iterator;
    delete start;
    delete end;
    delete body;
}

IfStmt::IfStmt(Expr* condition, Stmts* body, Token begin)
    : Stmt(NodeType::IfStmt, begin), condition(condition), body(body) {}
IfStmt::~IfStmt()
{
    delete condition;
    delete body;
}

AssignStmt::AssignStmt(Expr* lhs, Expr* rhs, Token begin)
    : Stmt(NodeType::AssignStmt, begin), lhs(lhs), rhs(rhs) {}
AssignStmt::~AssignStmt()
{
    delete lhs;
    delete rhs;
}

FunctionDeclaration::FunctionDeclaration(Identifier* name, Parameters* params, Stmts* body, Token begin)
    : Stmt(NodeType::FunctionDeclaration, begin), name(name), params(params), body(body), ref_count(1) {}
FunctionDeclaration::~FunctionDeclaration()
{
    delete name;
    delete params;
    delete body;
}

ReturnStmt::ReturnStmt(Expr* return_expr, Token begin)
    : Stmt(NodeType::ReturnStmt, begin), return_expr(return_expr) {}
ReturnStmt::~ReturnStmt()
{
    delete return_expr;
}

Arguments::Arguments(std::vector<Expr*> arguments, Token begin)
    : Stmt(NodeType::Arguments, begin), arguments(arguments) {}
Arguments::~Arguments()
{
    for (Expr* arg : arguments) {
        delete arg;
    }
}

Parameters::Parameters(std::vector<Identifier*> parameters, Token begin)
    : Stmt(NodeType::Parameters, begin), parameters(parameters) {}
Parameters::~Parameters()
{
    for (Identifier* param : parameters) {
        delete param;
    }
}

Expr::Expr(NodeType type, Token begin)
    : Stmt(type, begin) {}

BinaryExpr::BinaryExpr(Expr* lhs, Expr* rhs, Token op, Token begin)
    : Expr(NodeType::BinaryExpr, begin), lhs(lhs), rhs(rhs), op(op) {}
BinaryExpr::~BinaryExpr()
{
    delete lhs;
    delete rhs;
}

UnaryExpr::UnaryExpr(Expr* operand, Token op, Token begin)
    : Expr(NodeType::UnaryExpr, begin), operand(operand), op(op) {}
UnaryExpr::~UnaryExpr()
{
    delete operand;
}

NumericLiteral::NumericLiteral(double value, Token begin)
    : Expr(NodeType::NumericLiteral, begin), value(value) {}

Identifier::Identifier(std::string name, Token begin)
    : Expr(NodeType::Identifier, begin), name(name) {}

CallExpr::CallExpr(Identifier* callee, Arguments* arguments, Token begin)
    : Expr(NodeType::CallExpr, begin), callee(callee), arguments(arguments) {}
CallExpr::~CallExpr()
{
    delete callee;
    delete arguments;
}

MemberExpr::MemberExpr(Expr* object, Expr* index, Token begin)
    : Expr(NodeType::MemberExpr, begin), object(object), index(index) {}
MemberExpr::~MemberExpr()
{
    delete object;
    delete index;
}

ListDeclaration::ListDeclaration(std::vector<Expr*> elements, Token begin)
    : Expr(NodeType::ListDeclaration, begin), elements(elements) {}
ListDeclaration::~ListDeclaration()
{
    for (Expr* element : elements) {
        delete element;
    }
}

WaveDeclaration::WaveDeclaration(
        Expr* wave_expr,
        Expr* freq_expr,
        Expr* phase_expr,
        Expr* vol_expr,
        Expr* pan_expr,
        Token begin
        )
    : Expr(NodeType::WaveDeclaration, begin),
    wave_expr(wave_expr), freq_expr(freq_expr), phase_expr(phase_expr), vol_expr(vol_expr), pan_expr(pan_expr) {}
WaveDeclaration::~WaveDeclaration()
{
    // NOPE -Decrease each reference count and delete function if it reaches 0 NOPE
    delete wave_expr;
    delete freq_expr;
    delete phase_expr;
    delete vol_expr;
    delete pan_expr;
}


void printAST(Stmt* node, int indent, bool in_list)
{
    if (in_list)
        printIndent(indent);
    std::cout << "{\n";
    indent++;
    printIndent(indent);
    std::cout << "type: ";
    switch (node->type) {
        case NodeType::Program: {
            Program* dnode = (Program*)(node);
            std::cout << "Program\n";

            formatNode("body", dnode->body, indent);

            break;
        }
        case NodeType::Stmts: {
            Stmts* dnode = (Stmts*)(node);
            std::cout << "Stmts\n";

            printIndent(indent);
            std::cout << "stmts: [\n";
            for (Stmt* stmt : dnode->stmts)
                printAST(stmt, indent+1, true);
            
            printIndent(indent);
            std::cout << "]\n";
            
            break;
        }
        case NodeType::BinaryExpr: {
            BinaryExpr* dnode = (BinaryExpr*)(node);
            std::cout << "BinaryExpr\n";

            formatNode("lhs", dnode->lhs, indent);
            formatNode("rhs", dnode->rhs, indent);

            printIndent(indent);
            std::cout << "op: " << "\"" << dnode->op.value << "\"\n";

            break;
        }
        case NodeType::UnaryExpr: {
            UnaryExpr* dnode = (UnaryExpr*)(node);
            std::cout << "UnaryExpr\n";

            formatNode("operand", dnode->operand, indent);

            printIndent(indent);
            std::cout << "op: " << "\"" << dnode->op.value << "\"\n";

            break;
        }
        case NodeType::Identifier: {
            Identifier* dnode = (Identifier*)(node);
            std::cout << "Identifier\n";

            printIndent(indent);
            std::cout << "name: " << "\"" << dnode->name << "\"\n";

            break;
        }
        case NodeType::NumericLiteral: {
            NumericLiteral* dnode = (NumericLiteral*)(node);
            std::cout << "NumericLiteral\n";

            printIndent(indent);
            std::cout << "value: " << dnode->value << "\n";

            break;
        }
        case NodeType::Arguments: {
            Arguments* dnode = (Arguments*)(node);
            std::cout << "Arguments\n";

            printIndent(indent);
            std::cout << "arguments: [\n";

            for (Expr* arg : dnode->arguments)
                printAST(arg, indent+1, true);
            
            printIndent(indent);
            std::cout << "]\n";

            break;
        }
        case NodeType::Parameters: {
            Parameters* dnode = (Parameters*)(node);
            std::cout << "Parameters\n";

            printIndent(indent);
            std::cout << "parameters: [\n";

            for (Identifier* param : dnode->parameters)
                printAST(param, indent+1, true);
            
            printIndent(indent);
            std::cout << "]\n";
            
            break;
        }
        case NodeType::CallExpr: {
            CallExpr* dnode = (CallExpr*)(node);
            std::cout << "CallExpr\n";

            formatNode("callee", dnode->callee, indent);
            formatNode("arguments", dnode->arguments, indent);

            break;
        }
        case NodeType::MemberExpr: {
            MemberExpr* dnode = (MemberExpr*)(node);
            std::cout << "MemberExpr\n";

            formatNode("object", dnode->object, indent);
            formatNode("index", dnode->index, indent);

            break;
        }
        case NodeType::ListDeclaration: {
            ListDeclaration* dnode = (ListDeclaration*)(node);
            std::cout << "ListDeclaration\n";

            printIndent(indent);
            std::cout << "elements: [\n";

            for (Expr* element : dnode->elements)
                printAST(element, indent+1, true);

            printIndent(indent);
            std::cout << "]\n";

            break;
        }
        case NodeType::AssignStmt: {
            AssignStmt* dnode = (AssignStmt*)(node);
            std::cout << "AssignStmt\n";

            formatNode("lhs", dnode->lhs, indent);
            formatNode("rhs", dnode->rhs, indent);

            break;
        }
        case NodeType::IfStmt: {
            IfStmt* dnode = (IfStmt*)(node);
            std::cout << "IfStmt\n";

            formatNode("condition", dnode->condition, indent);
            formatNode("body", dnode->body, indent);

            break;
        }
        case NodeType::ForStmt: {
            ForStmt* dnode = (ForStmt*)(node);
            std::cout << "ForStmt\n";

            formatNode("iterator", dnode->iterator, indent);
            formatNode("start", dnode->start, indent);
            formatNode("end", dnode->end, indent);
            formatNode("body", dnode->body, indent);

            break;
        }
        case NodeType::FunctionDeclaration: {
            FunctionDeclaration* dnode = (FunctionDeclaration*)(node);
            std::cout << "FunctionDeclaration\n";

            formatNode("name", dnode->name, indent);
            formatNode("params", dnode->params, indent);
            formatNode("body", dnode->body, indent);

            break;
        }
        case NodeType::ReturnStmt: {
            ReturnStmt* dnode = (ReturnStmt*)(node);
            std::cout << "ReturnStmt\n";

            formatNode("return_val", dnode->return_expr, indent);

            break;
        }
        default:
            break;
    }
    indent--;
    printIndent(indent);
    if (in_list)
        std::cout << "},\n";
    else
        std::cout << "}\n";
}

void formatNode(std::string name, Stmt* node, int indent)
{
    printIndent(indent);
    std::cout << name << ": ";
    printAST(node, indent);
}

void printIndent(int depth)
{
    for (int i = 0; i < depth; i++) {
        std::cout << "  ";
    }
}