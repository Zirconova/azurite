#pragma once

#include <unordered_map>
#include <string>

#include "runtimeval.h"

class Environment
{
public:
    std::unordered_map<std::string, std::shared_ptr<RuntimeVal>> var_map;
    std::unordered_map<std::string, FunctionDeclaration*> func_map;

    Environment() {}
    ~Environment();

    std::shared_ptr<RuntimeVal> get_var(std::string name);
    void create_var(std::string name, std::shared_ptr<RuntimeVal> val);

    FunctionDeclaration* get_func(std::string name);
    void create_func(std::string name, FunctionDeclaration* func);
};