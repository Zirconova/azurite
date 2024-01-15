#include "environment.h"

Environment::~Environment()
{
    /*for (std::unordered_map<std::string, std::shared_ptr<RuntimeVal>>::iterator it = var_map.begin();
            it != var_map.end(); it++) {
        delete it->second;
    }*/
    for (std::unordered_map<std::string, FunctionDeclaration*>::iterator it = func_map.begin();
            it != func_map.end(); it++) {
        delete it->second;
    }
}

std::shared_ptr<RuntimeVal> Environment::get_var(std::string name)
{
    if (!var_map.count(name)) {
        std::cout << "Undeclared variable " << name << ".\n";
        exit(1);
    }
    return var_map[name];
}

void Environment::create_var(std::string name, std::shared_ptr<RuntimeVal> value)
{
    var_map[name] = value;
}

FunctionDeclaration* Environment::get_func(std::string name)
{
    if (!func_map.count(name)) {
        std::cout << "Undeclared function " << name << ".\n";
        exit(1);
    }
    return func_map[name];
}

void Environment::create_func(std::string name, FunctionDeclaration* func)
{
    func_map[name] = func;
}