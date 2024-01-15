#include "interpreter.h"

Interpreter::Interpreter()
{
    global_scope = new Environment();
    scopes.push_back(global_scope);
}

Interpreter::~Interpreter()
{
    delete global_scope;
}

RuntimeValPtr Interpreter::get_var(std::string name) {
    std::cout << "Checking for var, num scopes: " << scopes.size() << std::endl;
    for (std::vector<Environment*>::reverse_iterator it = scopes.rbegin(); it != scopes.rend(); it++) {
        if ((*it)->var_map.count(name)) {
            return (*it)->get_var(name);
        }
    }
    std::cout << "Undeclared variable " << name << std::endl;
    exit(1);
}

FunctionDeclaration* Interpreter::get_func(std::string name) {
    for (std::vector<Environment*>::reverse_iterator it = scopes.rbegin(); it != scopes.rend(); it++) {
        if ((*it)->func_map.count(name)) {
            return (*it)->get_func(name);
        }
    }
    std::cout << "Undeclared function " << name << std::endl;
    exit(1);
}

void Interpreter::exit_scope()
{
    std::cout << "about to exit scope from " << scopes.size() << " to ";
    delete scopes.back();
    scopes.pop_back();
    std::cout << scopes.size() << std::endl;
}

void Interpreter::interpret(std::string source)
{
    program = parser.parse(source);

    printAST(program);

    std::cout << "=======================\nbouta interpret\n";
    evaluate_stmt(program->body);
}

RuntimeValPtr Interpreter::evaluate_stmt(Stmt* node)
{
    RuntimeValPtr return_val = nullptr;

    std::cout << "in evaluate_stmt\n";

    switch (node->type) {
        case NodeType::Stmts: {
            std::cout << "bouta evaluate Stmts\n";
            return_val = evaluate_stmts((Stmts*)(node));
            break;
        }
        case NodeType::AssignStmt: {
            std::cout << "bouta interpret AssignStmt\n";
            interpret_assignstmt((AssignStmt*)(node));
            break;
        }
        case NodeType::IfStmt: {
            std::cout << "bouta evaluate IfStmt\n";
            return_val = evaluate_ifstmt((IfStmt*)(node));
            break;
        }
        case NodeType::ForStmt: {
            std::cout << "bouta evaluate ForStmt\n";
            return_val = evaluate_forstmt((ForStmt*)(node));
            break;
        }
        case NodeType::FunctionDeclaration: {
            std::cout << "bouta interpret FunctionDeclaration\n";
            interpret_functiondeclaration((FunctionDeclaration*)(node));
            break;
        }
        case NodeType::ReturnStmt: {
            std::cout << "bouta evaluate ReturnStmt\n";
            return_val = evaluate_returnstmt((ReturnStmt*)(node));
            break;
        }
        case NodeType::CallExpr: {
            std::cout << "bouta evaluate CallExpr\n";
            evaluate_callexpr((CallExpr*)(node));
            break;
        }
        case NodeType::BinaryExpr: {
            std::cout << "bouta evaluate BinaryExpr\n";
            evaluate_binaryexpr((BinaryExpr*)(node));
            break;
        }
        case NodeType::UnaryExpr: {
            std::cout << "bouta evaluate UnaryExpr\n";
            evaluate_unaryexpr((UnaryExpr*)(node));
            break;
        }
        case NodeType::MemberExpr: {
            std::cout << "bouta evaluate MemberExpr\n";
            evaluate_memberexpr((MemberExpr*)(node));
            break;
        }
        case NodeType::ListDeclaration: {
            std::cout << "bouta evaluate ListDeclaration\n";
            evaluate_listdeclaration((ListDeclaration*)(node));
            break;
        }
        default:
            std::cout << "never heard of this node g\n";
            break;
    }

    return return_val;
}

RuntimeValPtr Interpreter::evaluate_stmts(Stmts* node)
{
    RuntimeValPtr return_val = nullptr;

    for (Stmt* stmt : ((Stmts*)(node))->stmts) {
        return_val = evaluate_stmt(stmt);
        if (return_val) {
            break;
        }
    }

    return return_val;
}

void Interpreter::interpret_assignstmt(AssignStmt* node)
{
    if (node->lhs->type == NodeType::Identifier) {
        Identifier* lhs_id = (Identifier*)(node->lhs);
        scopes.back()->create_var(lhs_id->name, evaluate_expr(node->rhs));
    } else if (node->lhs->type == NodeType::MemberExpr) {
        MemberExpr* lhs_member = (MemberExpr*)(node->lhs);
        // Get double pointer to element indexed
        RuntimeValPtr* object = evaluate_memberexpr(lhs_member);
        // TODO FIX THIS WITH SHARED_PTRs -- I think this works now
        // reallocate new value in same element of vector
        *object = evaluate_expr(node->rhs);
    }
}

void Interpreter::interpret_functiondeclaration(FunctionDeclaration* node)
{
    scopes.back()->create_func(node->name->name, node);
}

RuntimeValPtr Interpreter::evaluate_ifstmt(IfStmt* node)
{
    RuntimeValPtr return_val = nullptr;

    bool condition = evaluate_expr(node->condition)->get_truth();

    if (condition) {
        return_val = evaluate_stmts(node->body);
    }

    return return_val;
}

RuntimeValPtr Interpreter::evaluate_forstmt(ForStmt* node)
{
    RuntimeValPtr return_val = nullptr;

    RuntimeValPtr start_val = evaluate_expr(node->start);
    RuntimeValPtr end_val = evaluate_expr(node->end);

    if (start_val->type != RuntimeType::Number || end_val->type != RuntimeType::Number) {
        // Error: for loop bounds must be numbers
        runtime_error("For loop bounds must be numbers.", node->start->begin);
    }

    std::shared_ptr<Number> start = std::dynamic_pointer_cast<Number>(start_val);
    std::shared_ptr<Number> end = std::dynamic_pointer_cast<Number>(end_val);

    // Create and enter for loop scope
    Environment* for_scope = new Environment();
    std::cout << "pushing new scope, now ";
    scopes.push_back(for_scope);
    std::cout << scopes.size() << " scopes.\n";

    // Create iterator variable in for loop scope
    scopes.back()->create_var(node->iterator->name, start);

    // Run for loop
    for (int i = start->value; i < end->value; i++) {
        // Update iterator value
        scopes.back()->create_var(node->iterator->name, std::make_shared<Number>(i));

        return_val = evaluate_stmts(node->body);

        if (return_val) {
            break;
        }
    }

    exit_scope();

    return return_val;
}

RuntimeValPtr Interpreter::evaluate_returnstmt(ReturnStmt* node)
{
    return evaluate_expr(node->return_val);
}

RuntimeValPtr Interpreter::evaluate_expr(Expr* node)
{
    switch (node->type) {
        case NodeType::NumericLiteral: {
            return evaluate_numericliteral((NumericLiteral*)(node));
            break;
        }
        case NodeType::Identifier: {
            return evaluate_identifier((Identifier*)(node));
            break;
        }
        case NodeType::CallExpr: {
            RuntimeValPtr return_val = evaluate_callexpr((CallExpr*)(node));

            if (return_val) {
                return return_val;
            }
            // Error: non-returning function cannot be evaluated
            runtime_error("Non-returning function cannot be evaluated.", node->begin);
            break;
        }
        case NodeType::MemberExpr: {
            return *evaluate_memberexpr((MemberExpr*)(node));
            break;
        }
        case NodeType::BinaryExpr: {
            return evaluate_binaryexpr((BinaryExpr*)(node));
            break;
        }
        case NodeType::UnaryExpr: {
            return evaluate_unaryexpr((UnaryExpr*)(node));
            break;
        }
        case NodeType::ListDeclaration: {
            return evaluate_listdeclaration((ListDeclaration*)(node));
            break;
        }
        default: {
            std::cout << "Expression type not yet supported.\n";
            exit(1);
            break;
        }
    }
}

RuntimeValPtr Interpreter::evaluate_identifier(Identifier* node)
{
    return get_var(node->name);
}

RuntimeValPtr Interpreter::evaluate_numericliteral(NumericLiteral* node)
{
    return std::make_shared<Number>(node->value);
}

RuntimeValPtr Interpreter::evaluate_callexpr(CallExpr* node)
{
    RuntimeValPtr return_val = nullptr;

    Identifier* callee = node->callee;
    Arguments* args = node->arguments;
    std::vector<RuntimeValPtr> arg_vals;
    
    // Create and enter function scope
    Environment* func_scope = new Environment();
    std::cout << "pushing new scope, now";
    scopes.push_back(func_scope);
    std::cout << scopes.size() << " scopes\n";

    // Initialize arguments as variables in function scope
    for (Expr* arg : args->arguments) {
        arg_vals.push_back(evaluate_expr(arg));
    }

    // Run built-in function if it exists
    if (Azurite::builtins.count(callee->name)) {
       return_val = Azurite::call_runtimelib(callee->name, arg_vals);
    }
    // Else look for FunctionDeclaration in environment
    else {
        FunctionDeclaration* func = get_func(callee->name);
        // Assuming all params are Identifier expressions
        if (func->params->parameters.size() != arg_vals.size()) {
            // Error: length of argument list does not match parameter list
            runtime_error("Length of argument list does not match parameter list.", node->arguments->begin);
        }
        // Initialize scope with param names mapped to arg values
        for (int i = 0; i < arg_vals.size(); i++) {
            Identifier* param = func->params->parameters[i];
            scopes.back()->create_var(param->name, arg_vals[i]);
        }
        // Run function body
        return_val = evaluate_stmt(func->body);
    }

    exit_scope();

    return return_val;
}

RuntimeValPtr* Interpreter::evaluate_memberexpr(MemberExpr* node)
{
    RuntimeValPtr object = evaluate_expr(node->object);
    if (object->type == RuntimeType::List) {
        std::shared_ptr<List> object_list = std::dynamic_pointer_cast<List>(object);
        RuntimeValPtr index = evaluate_expr(node->index);
        if (index->type == RuntimeType::Number) {
            std::shared_ptr<Number> index_num = std::dynamic_pointer_cast<Number>(index);
            return &(object_list->elements[(int)(index_num->value)]);
        }
        // Error: list index must be number
        runtime_error("List index must be number.", node->index->begin);
    }
    // Error: only lists can be indexed
    runtime_error("Only lists can be indexed.", node->begin);
}

RuntimeValPtr Interpreter::evaluate_binaryexpr(BinaryExpr* node)
{
    RuntimeValPtr lhs_val = evaluate_expr(node->lhs);
    RuntimeValPtr rhs_val = evaluate_expr(node->rhs);

    std::string op = node->op.value;

    if (node->op.type == TokenType::ArithmeticOperator
     || node->op.type == TokenType::ComparisonOperator) {
        if (lhs_val->type == RuntimeType::Number
            && rhs_val->type == RuntimeType::Number) {
            std::shared_ptr<Number> lhs_num = std::dynamic_pointer_cast<Number>(lhs_val);
            std::shared_ptr<Number> rhs_num = std::dynamic_pointer_cast<Number>(rhs_val);
            // Arithmetic operators
            if (op == "+") {
                return std::make_shared<Number>(lhs_num->value + rhs_num->value);
            } else if (op == "-") {
                return std::make_shared<Number>(lhs_num->value - rhs_num->value);
            } else if (op == "*") {
                return std::make_shared<Number>(lhs_num->value * rhs_num->value);
            } else if (op == "/") {
                return std::make_shared<Number>(lhs_num->value / rhs_num->value);
            } else if (op == "%") {
                return std::make_shared<Number>(fmod(lhs_num->value, rhs_num->value));
            } else if (op == "^") {
                return std::make_shared<Number>(pow(lhs_num->value, rhs_num->value));
            // Comparison operators
            } else if (op == "==") {
                return std::make_shared<Bool>(lhs_num->value == rhs_num->value);
            } else if (op == "!=") {
                return std::make_shared<Bool>(lhs_num->value != rhs_num->value);
            } else if (op == ">") {
                return std::make_shared<Bool>(lhs_num->value > rhs_num->value);
            } else if (op == "<") {
                return std::make_shared<Bool>(lhs_num->value < rhs_num->value);
            } else if (op == ">=") {
                return std::make_shared<Bool>(lhs_num->value >= rhs_num->value);
            } else if (op == "<=") {
                return std::make_shared<Bool>(lhs_num->value <= rhs_num->value);
            }
            // Error: unknown operator
            runtime_error("Unknown operator.", node->op);
        }
        // Error: Arithmetic or comparison expressions must use only numbers
        runtime_error("Arithmetic or comparison expressions must use numbers only.", node->begin);

    } else if (node->op.type == TokenType::LogicalOperator) {
        if (op == "|") {
            return std::make_shared<Bool>(lhs_val->get_truth() | rhs_val->get_truth());
        } else if (op == "&") {
            return std::make_shared<Bool>(lhs_val->get_truth() & rhs_val->get_truth());
        }
        // Error: unknown operator
        runtime_error("Unknown operator.", node->op);
    }
}

RuntimeValPtr Interpreter::evaluate_unaryexpr(UnaryExpr* node)
{
    RuntimeValPtr operand_val = evaluate_expr(node->operand);

    std::string op = node->op.value;

    if (node->op.type == TokenType::LogicalOperator) {
        if (op == "!") {
            // Cast to bool and return negation
            return std::make_shared<Bool>(!operand_val->get_truth());
        }
        // Error: unknown operator
        runtime_error("Unknown operator.", node->op);

    } else if (node->op.type == TokenType::ArithmeticOperator) {
        if (operand_val->type == RuntimeType::Number) {
            std::shared_ptr<Number> operand_num = std::dynamic_pointer_cast<Number>(operand_val);
            if (op == "+") {
                return std::make_shared<Number>(operand_num->value);
            } else if (op == "-") {
                return std::make_shared<Number>(-operand_num->value);
            }
            // Error: unknown operator
            runtime_error("Unknown operator.", node->op);
        }
        // Error: arithmetic unary expressions must use only numbers
        runtime_error("Arithmetic unary expressions must use numbers only.", node->begin);
    }
}

RuntimeValPtr Interpreter::evaluate_listdeclaration(ListDeclaration* node)
{
    std::vector<RuntimeValPtr> elements;

    for (Expr* element : node->elements) {
        elements.push_back(evaluate_expr(element));
    }

    return std::make_shared<List>(elements);
}