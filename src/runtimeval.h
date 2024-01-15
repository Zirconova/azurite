#pragma once

#include <vector>
#include <memory>

#include "ast.h"

enum class RuntimeType
{
    Number,
    Bool,
    List,
    Wave
};


class RuntimeVal
{
public:
    RuntimeType type;

    RuntimeVal(RuntimeType type);
    virtual ~RuntimeVal() {}
    virtual bool get_truth() = 0;
};


class Number : public RuntimeVal
{
public:
    double value;

    Number(double value);
    ~Number() {}

    bool get_truth();
};


class Bool : public RuntimeVal
{
public:
    bool value;

    Bool(bool value);
    ~Bool() {}

    bool get_truth();
};


class List : public RuntimeVal
{
public:
    std::vector<std::shared_ptr<RuntimeVal>> elements;

    List(std::vector<std::shared_ptr<RuntimeVal>> elements);
    ~List();

    bool get_truth();
};


class Wave : public RuntimeVal
{
public:
    // TODO

    Wave();
    ~Wave();

    bool get_truth();
};

