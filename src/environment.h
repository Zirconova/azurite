#pragma once

#include <iostream>
#include <unordered_map>
#include <string>
#include <memory>

#include "ast.h"
#include "runtimeval.h"

typedef std::shared_ptr<RuntimeVal> RuntimeValPtr;

class Environment
{
public:
    std::unordered_map<std::string, RuntimeValPtr> var_map;
    std::unordered_map<std::string, FunctionDeclaration*> func_map;

    Environment() {}
    ~Environment();

    RuntimeValPtr get_var(std::string name);
    void create_var(std::string name, RuntimeValPtr val);

    FunctionDeclaration* get_func(std::string name);
    void create_func(std::string name, FunctionDeclaration* func);
};