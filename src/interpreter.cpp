#include "interpreter.h"

#define PI 3.14159265358979323846
#define TAU 6.28318530717958647692

Interpreter::Interpreter()
{
    Azurite::initialize_runtimelib();
    global_scope = new Environment();
    wave_buffer = new short[1000000];
    scopes.push_back(global_scope);
}

Interpreter::~Interpreter()
{
    delete global_scope;
    delete [] wave_buffer;
}

RuntimeValPtr Interpreter::get_var(std::string name) {
    //std::cout << "Checking for var, num scopes: " << scopes.size() << std::endl;
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

void Interpreter::create_var(std::string name, RuntimeValPtr value)
{
    // Try to reassign existing variable first
    for (std::vector<Environment*>::reverse_iterator it = scopes.rbegin(); it != scopes.rend(); it++) {
        if ((*it)->var_map.count(name)) {
            (*it)->create_var(name, value);
            return;
        }
    }

    // If no existing variable, create in local scope
    scopes.back()->create_var(name, value);
}

void Interpreter::create_func(std::string name, FunctionDeclaration* func)
{
    // Try to reassign existing function first
    for (std::vector<Environment*>::reverse_iterator it = scopes.rbegin(); it != scopes.rend(); it++) {
        if ((*it)->func_map.count(name)) {
            delete (*it)->func_map[name];
            (*it)->create_func(name, func);
            return;
        }
    }

    // If no existing function, create in local scope
    scopes.back()->create_func(name, func);
}

void Interpreter::new_scope()
{
    //std::cout << "pushing new scope, now ";
    Environment* scope = new Environment();
    scopes.push_back(scope);
    //std::cout << scopes.size() << " scopes.\n";
}

void Interpreter::exit_scope()
{
    //std::cout << "about to exit scope from " << scopes.size() << " to ";
    delete scopes.back();
    scopes.pop_back();
    //std::cout << scopes.size() << std::endl;
}

void Interpreter::interpret(std::string source)
{
    program = parser.parse(source);

    printAST(program);

    std::cout << "=======================\nbouta interpret\n";
    evaluate_stmt(program->body);

    write_wave_file("test.wav", wave_buffer, 1000000, 1);
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
        create_var(lhs_id->name, evaluate_expr(node->rhs));
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
    create_func(node->name->name, node);
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
    new_scope();

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
    return evaluate_expr(node->return_expr);
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
        case NodeType::WaveDeclaration: {
            return evaluate_wavedeclaration((WaveDeclaration*)(node));
            break;
        }
        default: {
            runtime_error("Expression type not yet supported.", node->begin);
            break;
        }
    }
}

RuntimeValPtr Interpreter::evaluate_identifier(Identifier* node)
{
    RuntimeValPtr value = get_var(node->name);

    if (value->type == RuntimeType::Wave) {
        std::shared_ptr<Wave> value_wave = std::dynamic_pointer_cast<Wave>(value);

        return get_sample_and_advance(value_wave);
    }

    return value;
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
    new_scope();

    // Initialize arguments as variables in function scope
    for (Expr* arg : args->arguments) {
        arg_vals.push_back(evaluate_expr(arg));
    }

    // Run built-in function if it exists
    if (callee->name == "write") {
        return_val = write_wave(arg_vals);
    }
    else if (Azurite::has_builtin(callee->name)) {
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

    if (object->type != RuntimeType::List) {
        // Error: only lists can be indexed
        runtime_error("Only lists can be indexed.", node->begin);
    }

    std::shared_ptr<List> object_list = std::dynamic_pointer_cast<List>(object);
    RuntimeValPtr index = evaluate_expr(node->index);

    if (index->type != RuntimeType::Number) {
        // Error: list index must be number
        runtime_error("List index must be number.", node->index->begin);
    }

    std::shared_ptr<Number> index_num = std::dynamic_pointer_cast<Number>(index);

    if (index_num->value >= object_list->elements.size() || index_num->value < 0) {
        // Error: list index out of range
        runtime_error("List index out of range.", node->index->begin);
    }

    return &(object_list->elements[(int)(index_num->value)]);
}

RuntimeValPtr Interpreter::evaluate_binaryexpr(BinaryExpr* node)
{
    RuntimeValPtr lhs_val = evaluate_expr(node->lhs);
    RuntimeValPtr rhs_val = evaluate_expr(node->rhs);

    std::string op = node->op.value;

    if (node->op.type == TokenType::ArithmeticOperator
        || node->op.type == TokenType::ComparisonOperator) {
            
        if (lhs_val->type != RuntimeType::Number
            || rhs_val->type != RuntimeType::Number) {
            // Error: Arithmetic or comparison expressions must use only numbers
            runtime_error("Arithmetic or comparison expressions must use numbers only.", node->begin);
        }

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
        if (operand_val->type != RuntimeType::Number) {
            // Error: arithmetic unary expressions must use only numbers
            runtime_error("Arithmetic unary expressions must use numbers only.", node->begin);
        }

        std::shared_ptr<Number> operand_num = std::dynamic_pointer_cast<Number>(operand_val);

        if (op == "+") {
            return std::make_shared<Number>(operand_num->value);
        } else if (op == "-") {
            return std::make_shared<Number>(-operand_num->value);
        }
        // Error: unknown operator
        runtime_error("Unknown operator.", node->op);
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

RuntimeValPtr Interpreter::evaluate_wavedeclaration(WaveDeclaration* node)
{
    std::cout << "evaluating wavedeclaration\n";
    Expr* wave_expr = node->wave_expr;
    Expr* freq_expr = node->freq_expr;
    Expr* phase_expr = node->phase_expr;
    Expr* vol_expr = node->vol_expr;
    Expr* pan_expr = node->pan_expr;

    return std::make_shared<Wave>(wave_expr, freq_expr, phase_expr, vol_expr, pan_expr);
}

RuntimeValPtr Interpreter::write_wave(std::vector<RuntimeValPtr> args)
{
    if (args[0]->type != RuntimeType::Wave) {
        std::cout << "Only Wave objects can be written.\n";
        return nullptr;
    }

    if (args.size() > 1 && args[1]->type != RuntimeType::Number) {
        std::cout << "Length must be a number.\n";
        return nullptr;
    }

    std::shared_ptr<Wave> wave = std::dynamic_pointer_cast<Wave>(args[0]);
    std::shared_ptr<Number> length = std::dynamic_pointer_cast<Number>(args[1]);

    // Write each sample to buffer
    for (int i = 0; i < length->value; i++) {
        // TODO: Put this in buffer
        double sample = get_sample_and_advance(wave)->value;
        std::cout << sample << std::endl;
        wave_buffer[i] = sample * 32768;
    }

    return nullptr;
}

std::shared_ptr<Number> Interpreter::get_sample_and_advance(std::shared_ptr<Wave> wave)
{
    // Create and enter new scope
    new_scope();

    // Evaluate height of wave with // TODO add panning
    // height = waveform(phase + phaseoffset) * vol
    scopes.back()->create_var("x", std::make_shared<Number>(wave->sample));
    RuntimeValPtr freq = evaluate_expr(wave->freq_expr);
    RuntimeValPtr phase_offset = evaluate_expr(wave->phase_expr);
    RuntimeValPtr vol = evaluate_expr(wave->vol_expr);

    if (freq->type != RuntimeType::Number
        || phase_offset->type != RuntimeType::Number
        || vol->type != RuntimeType::Number) {
        
        std::cout << "All wave functions must evaluate to numbers.\n";
        return nullptr;
    }

    std::shared_ptr<Number> freq_num = std::dynamic_pointer_cast<Number>(freq);
    std::shared_ptr<Number> phase_offset_num = std::dynamic_pointer_cast<Number>(phase_offset);
    std::shared_ptr<Number> vol_num = std::dynamic_pointer_cast<Number>(vol);

    scopes.back()->create_var("x", std::make_shared<Number>(wave->phase + phase_offset_num->value));
    RuntimeValPtr height = evaluate_expr(wave->wave_expr);

    if (height->type != RuntimeType::Number) {
        std::cout << "All wave functions must evaluate to numbers.\n";
        return nullptr;
    }

    std::shared_ptr<Number> height_num = std::dynamic_pointer_cast<Number>(height);

    std::shared_ptr<Number> final_height = std::make_shared<Number>(height_num->value * vol_num->value);

    // Advance
    wave->sample++;

    // phase += 2pi*(freq at sample)/samplerate
    wave->phase += TAU * (freq_num->value) / 44100;

    exit_scope();

    return final_height;
}