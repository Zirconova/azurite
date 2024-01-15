#include "runtimeval.h"

RuntimeVal::RuntimeVal(RuntimeType type)
    : type(type) {}

Number::Number(double value)
    : RuntimeVal(RuntimeType::Number), value(value) {}
bool Number::get_truth()
{
    return (bool)value;
}

Bool::Bool(bool value)
    : RuntimeVal(RuntimeType::Bool), value(value) {}
bool Bool::get_truth()
{
    return value;
}

List::List(std::vector<std::shared_ptr<RuntimeVal>> elements)
    : RuntimeVal(RuntimeType::List), elements(elements) {}
List::~List()
{
    // for (std::shared_ptr<RuntimeVal> element : elements) {
    //     delete element;
    // }
}
bool List::get_truth()
{
    return !elements.empty();
}

Wave::Wave() // TODO
    : RuntimeVal(RuntimeType::Wave) {}
Wave::~Wave()
{
    // TODO
}
bool Wave::get_truth()
{
    return true;
}