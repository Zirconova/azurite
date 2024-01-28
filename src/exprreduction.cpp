#include "exprreduction.h"

RuntimeValPointerNode::RuntimeValPointerNode(std::shared_ptr<RuntimeVal> value, Token begin)
    : Expr(NodeType::RuntimeValPointerNode, begin), value(value) {}

NumberPointerNode::NumberPointerNode(double* value, Token begin)
    : Expr(NodeType::NumberPointerNode, begin), value(value) {}