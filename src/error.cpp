#include "error.h"

void runtime_error(std::string msg, Token token)
{
    std::cout << "Runtime error: " << msg;
    std::cout << " Line " << token.line << ", col " << token.col << std::endl;
    exit(1);
}