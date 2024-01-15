#include <fstream>
#include <sstream>

#include "interpreter.h"

int main(int argc, char* argv[]) {
    // std::string src = "notes = ([0,2,3,5,7,10])\n"
    // "func next(list,i) {\n"
    // "print(list[i])\n"
    // "next(list,i+1)\n"
    // "}\n\n"
    // "next(notes,0)\n";

    std::string src = "x = 0\n"
    "y = 0\n"
    "if x | y {\n"
    "print(100)\n"
    "}\n"
    "if x <= y & !y {\n"
    "print(50)\n"
    "}";

    // Read source file (first command line argument)
    std::ifstream source_file(argv[1]);
    std::stringstream buffer;
    buffer << source_file.rdbuf();

    Interpreter interpreter;

    interpreter.interpret(buffer.str());
}