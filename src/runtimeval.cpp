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

String::String(std::string value)
    : RuntimeVal(RuntimeType::String), value(value) {}
bool String::get_truth()
{
    return value != "";
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

Wave::Wave(
        Expr* wave_expr,
        Expr* freq_expr,
        Expr* phase_expr,
        Expr* vol_expr,
        Expr* pan_expr
        )
    : RuntimeVal(RuntimeType::Wave), phase(0.0), sample(0),
    wave_expr(wave_expr), freq_expr(freq_expr), phase_expr(phase_expr), vol_expr(vol_expr), pan_expr(pan_expr) {}

bool Wave::get_truth()
{
    return true;
}