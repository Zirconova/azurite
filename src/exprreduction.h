#pragma once

#include <memory>

#include "ast.h"
#include "token.h"
#include "runtimeval.h"

class RuntimeValPointerNode : public Expr
{
public:
    std::shared_ptr<RuntimeVal> value;

    RuntimeValPointerNode(std::shared_ptr<RuntimeVal> value, Token begin);
    ~RuntimeValPointerNode() {}
};


class NumberPointerNode : public Expr
{
public:
    double* value;

    NumberPointerNode(double* value, Token begin);
    ~NumberPointerNode() {}
};