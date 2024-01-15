#include "runtimeval.h"

typedef std::shared_ptr<RuntimeVal> RuntimeValPtr;

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

List::List(std::vector<RuntimeValPtr> elements)
    : RuntimeVal(RuntimeType::List), elements(elements) {}
List::~List()
{
    // for (RuntimeValPtr element : elements) {
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