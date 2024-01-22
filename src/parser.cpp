#include "parser.h"

// Get token at cursor
Token Parser::at()
{
    return tokens[ptr];
}

// Get token at cursor and advance cursor
Token Parser::eat()
{
    return tokens[ptr++];
}

// Eat, or throw an error if the token is not the expected type
Token Parser::expect(TokenType type, std::string msg)
{
    if (at().type != type) {
        syntax_error(msg);
    }

    return eat();
}

void Parser::syntax_error(std::string msg)
{
    std::cout << "Syntax error: " << msg;
    std::cout << " Line " << at().line << ", col " << at().col << std::endl;
    exit(1);
}

// Advance to first non-endline token
void Parser::skip_whitespace()
{
    std::cout << "skipping whitespace\n";
    while (at().type == TokenType::Endline) {
        eat();
    }
}

// Return token 1 beyond current cursor position
Token Parser::peek()
{
    if (ptr + 1 < tokens.size())
        return tokens[ptr + 1];
}

Program* Parser::parse(std::string source)
{
    ptr = 0;
    tokens = lexer.tokenize(source);

    for (Token token : tokens) {
        std::cout << token.value << ",\n";
    }

    Token begin = at();
    Stmts* body = parse_stmts();

    return new Program(body, begin);
}

Stmts* Parser::parse_stmts()
{
    Token begin = at();
    std::vector<Stmt*> stmts;

    std::cout << "parsing stmts\n";

    while (at().type != TokenType::EndOfFile && at().type != TokenType::CloseBrace) {
        std::cout << "adding stmts, token is: \"" << at().value << "\"\n";
        stmts.push_back(parse_stmt());
    }

    return new Stmts(stmts, begin);
}

Stmts* Parser::parse_block()
{
    Stmts* block;

    expect(TokenType::OpenBrace, "Expected '{' at start of block.");

    skip_whitespace();

    if (at().type != TokenType::CloseBrace) {
        block = parse_stmts();
    }

    expect(TokenType::CloseBrace, "Expected '}' at end of block.");

    return block;
}

Stmt* Parser::parse_stmt()
{
    skip_whitespace();

    std::cout << "parsing stmt\n";

    Stmt* result;

    if (at().type == TokenType::Identifier) {
        if (peek().type == TokenType::OpenSquare || peek().type == TokenType::Equals) {
            result = parse_assignstmt();
        } else {
            result = parse_expr();
        }
    } else if (at().type == TokenType::If) {
        result = parse_ifstmt();
    } else if (at().type == TokenType::For) {
        result = parse_forstmt();
    } else if (at().type == TokenType::Func) {
        result = parse_functiondeclaration();
    } else if (at().type == TokenType::Return) {
        result = parse_returnstmt();
    } else {
        result = parse_expr();
    }

    expect(TokenType::Endline, "Expected newline after statement.");

    skip_whitespace();

    return result;
}

AssignStmt* Parser::parse_assignstmt()
{
    Token begin = at();

    Expr* lhs = parse_memberexpr();

    std::cout << "parsing assignexpr\n";

    expect(TokenType::Equals, "Expected '='.");

    Expr* rhs = parse_expr();

    return new AssignStmt(lhs, rhs, begin);
}

IfStmt* Parser::parse_ifstmt()
{
    Token begin = at();

    // Eat 'if' keyword
    eat();

    Expr* condition = parse_expr();

    Stmts* body = parse_block();

    return new IfStmt(condition, body, begin);
}

ForStmt* Parser::parse_forstmt()
{
    Token begin = at();

    // Eat 'for' keyword
    eat();

    Token id_begin = at();
    std::string name = expect(TokenType::Identifier, "Expected identifier in for loop header.").value;

    Identifier* iterator = new Identifier(name, id_begin);

    expect(TokenType::OpenParen, "Expected '('.");

    Expr* start = parse_expr();

    expect(TokenType::Comma, "Expected ','.");

    Expr* end = parse_expr();

    expect(TokenType::CloseParen, "Expected ')'.");

    Stmts* body = parse_block();

    return new ForStmt(iterator, start, end, body, begin);
}

ReturnStmt* Parser::parse_returnstmt()
{
    Token begin = at();

    // Eat 'return' keyword
    eat();

    return new ReturnStmt(parse_expr(), begin);
}

FunctionDeclaration* Parser::parse_functiondeclaration()
{
    Token begin = at();
    // Eat 'func' keyword
    eat();

    Token id_begin = at();
    std::string func_name = expect(TokenType::Identifier, "Expected identifier in function header.").value;
    Identifier* name = new Identifier(func_name, id_begin);

    Parameters* params = parse_parameters();

    Stmts* body = parse_block();

    return new FunctionDeclaration(name, params, body, begin);
}

Parameters* Parser::parse_parameters()
{
    Token begin = at();
    std::vector<Identifier*> parameters;

    expect(TokenType::OpenParen, "Expected '('.");

    if (at().type != TokenType::CloseParen) {
        bool start = true;
        do {
            // Eat comma if not at start
            if (!start) {
                eat();
            }
            start = false;

            // Make parameter identifier
            Token id_begin = at();
            std::string param_name = expect(TokenType::Identifier, "Expected an identifier.").value;
            Identifier* param = new Identifier(param_name, id_begin);

            parameters.push_back(param);

        } while (at().type == TokenType::Comma);
    }

    expect(TokenType::CloseParen, "Expected ')'.");

    return new Parameters(parameters, begin);
}

Arguments* Parser::parse_arguments()
{
    Token begin = at();
    std::vector<Expr*> arguments;

    expect(TokenType::OpenParen, "Expected '('.");

    if (at().type != TokenType::CloseParen) {
        bool start = true;
        do {
            // Eat comma if not at start
            if (!start) {
                eat();
            }
            start = false;

            Expr* arg = parse_expr();

            arguments.push_back(arg);

        } while (at().type == TokenType::Comma);
    }

    expect(TokenType::CloseParen, "Expected ')'.");

    return new Arguments(arguments, begin);
}

Expr* Parser::parse_expr()
{
    std::cout << "parsing expr\n";

    return parse_or();
}

Expr* Parser::parse_or()
{
    Token begin = at();
    Expr* lhs = parse_and();

    while (at().value == "|") {
        Token op = eat();
        Expr* rhs = parse_and();
        lhs = new BinaryExpr(lhs, rhs, op, begin);
    }

    return lhs;
}

Expr* Parser::parse_and()
{
    Token begin = at();
    Expr* lhs = parse_comp();

    while (at().value == "&") {
        Token op = eat();
        Expr* rhs = parse_comp();
        lhs = new BinaryExpr(lhs, rhs, op, begin);
    }

    return lhs;
}

Expr* Parser::parse_comp()
{
    Token begin = at();
    Expr* lhs = parse_sum();

    while (at().type == TokenType::ComparisonOperator) {
        Token op = eat();
        Expr* rhs = parse_sum();
        lhs = new BinaryExpr(lhs, rhs, op, begin);
    }

    return lhs;
}

Expr* Parser::parse_sum()
{
    Token begin = at();
    Expr* lhs = parse_term();

    while (at().value == "+" || at().value == "-") {
        Token op = eat();
        Expr* rhs = parse_term();
        lhs = new BinaryExpr(lhs, rhs, op, begin);
    }

    return lhs;
}

Expr* Parser::parse_term()
{
    Token begin = at();
    Expr* lhs = parse_factor();

    while (at().value == "*" || at().value == "/" || at().value == "%") {
        Token op = eat();
        Expr* rhs = parse_factor();
        lhs = new BinaryExpr(lhs, rhs, op, begin);
    }

    return lhs;
}

Expr* Parser::parse_factor()
{
    Token begin = at();
    Expr* lhs = parse_unaryexpr();

    while (at().value == "^") {
        Token op = eat();
        Expr* rhs = parse_unaryexpr();
        lhs = new BinaryExpr(lhs, rhs, op, begin);
    }

    return lhs;
}

Expr* Parser::parse_unaryexpr()
{
    Token begin = at();

    if (at().value == "-" || at().value == "+" || at().value == "!") {
        Token op = eat();
        Expr* operand = parse_primaryexpr();
        return new UnaryExpr(operand, op, begin);
    }

    return parse_primaryexpr();
}

Expr* Parser::parse_primaryexpr()
{
    Token begin = at();

    std::cout << "parsing primary\n";

    if (at().type == TokenType::Number) {
        return new NumericLiteral(std::stod(eat().value), begin);

    } else if (at().type == TokenType::Identifier) {
        if (peek().type == TokenType::OpenParen) {
            return parse_callexpr();
        }
        return parse_memberexpr();

    } else if (at().type == TokenType::Wave) {
        return parse_wavedeclaration();

    } else if (at().type == TokenType::OpenSquare) {
        return parse_listdeclaration();

    } else if (at().type == TokenType::OpenParen) {
        eat();
        Expr* value = parse_expr();
        expect(TokenType::CloseParen, "Expected ')'.");
        return value;

    } else {
        syntax_error("Unexpected token '" + at().value + "'.");
    }
}

Expr* Parser::parse_memberexpr()
{
    Token begin = at();
    Expr* lhs = new Identifier(eat().value, begin);

    while (at().type == TokenType::OpenSquare) {
        // Eat opening parenthesis
        eat();
        Expr* index = parse_expr();
        expect(TokenType::CloseSquare, "Expected ']'.");
        lhs = new MemberExpr(lhs, index, begin);
    }

    return lhs;
}

CallExpr* Parser::parse_callexpr()
{
    Token begin = at();
    Identifier* callee = new Identifier(eat().value, begin);

    Arguments* arguments = parse_arguments();

    return new CallExpr(callee, arguments, begin);
}

ListDeclaration* Parser::parse_listdeclaration()
{
    Token begin = at();

    // Eat opening square
    eat();

    std::vector<Expr*> elements;

    skip_whitespace();

    while (at().type != TokenType::CloseSquare) {
        Expr* element = parse_expr();
        elements.push_back(element);
        skip_whitespace();
        if (at().type != TokenType::CloseSquare) {
            expect(TokenType::Comma, "Expected ','.");
        }
        skip_whitespace();
    }

    // Eat closing square
    eat();

    return new ListDeclaration(elements, begin);
}

WaveDeclaration* Parser::parse_wavedeclaration()
{
    Token begin = at();

    // Eat 'Wave' keyword
    eat();

    expect(TokenType::OpenParen, "Expected '('.");

    skip_whitespace();

    Expr* default_wave = new CallExpr(
        new Identifier("sin", at()),
        new Arguments({new Identifier("x", at())}, at()),
        at()
    );
    Expr* default_freq = new NumericLiteral(0.0, at());
    Expr* default_phase = new NumericLiteral(0.0, at());
    Expr* default_vol = new NumericLiteral(1.0, at());
    Expr* default_pan = new NumericLiteral(0.0, at());

    Expr* wave_expr = default_wave;
    Expr* freq_expr = default_freq;
    Expr* phase_expr = default_phase;
    Expr* vol_expr = default_vol;
    Expr* pan_expr = default_pan;

    while (at().type == TokenType::Identifier) {
        std::string type = eat().value;

        expect(TokenType::Colon, "Expected ':'.");

        Expr* function_expr = parse_expr();

        if (type == "waveform") {
            delete wave_expr;
            wave_expr = function_expr;
        } else if (type == "freq") {
            delete freq_expr;
            freq_expr = function_expr;
        } else if (type == "phase") {
            delete phase_expr;
            phase_expr = function_expr;
        } else if (type == "vol") {
            delete vol_expr;
            vol_expr = function_expr;
        } else if (type == "pan") {
            delete pan_expr;
            pan_expr = function_expr;
        } else {
            syntax_error("Unrecognized wave function specifier.");
        }

        if (at().type == TokenType::Comma) {
            eat();
            skip_whitespace();
        }
    }

    expect(TokenType::CloseParen, "Expected ')'.");

    return new WaveDeclaration(wave_expr, freq_expr, phase_expr, vol_expr, pan_expr, begin);
}

