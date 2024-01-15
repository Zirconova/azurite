#include "runtimelib.h"

std::unordered_set<std::string> Azurite::builtins = {"print", "sin"};

RuntimeValPtr Azurite::call_runtimelib(std::string name, std::vector<RuntimeValPtr>& args)
{
    if (name == "print") {
        return Azurite::print(args);
    } else if (name == "sin") {
        return Azurite::sin(args);
    }
}

RuntimeValPtr Azurite::print(std::vector<RuntimeValPtr>& args)
{
    for (RuntimeValPtr arg : args) {
        switch(arg->type) {
            case RuntimeType::Number: {
                std::shared_ptr<Number> arg_num = std::dynamic_pointer_cast<Number>(arg);
                std::cout << arg_num->value << " ";
                break;
            }
            case RuntimeType::Bool: {
                std::shared_ptr<Bool> arg_bool = std::dynamic_pointer_cast<Bool>(arg);
                std::cout << arg_bool->get_truth() << " ";
                break;
            }
            default:
                std::cout << "Unprintable datatype.";
                break;
        }
    }

    std::cout << '\n';

    return nullptr;
}

RuntimeValPtr Azurite::sin(std::vector<RuntimeValPtr>& args)
{
    if (args[0]->type == RuntimeType::Number) {
        std::shared_ptr<Number> arg_num = std::dynamic_pointer_cast<Number>(args[0]);
        return std::make_shared<Number>(std::sin(arg_num->value));
    } else {
        std::cout << "Cannot take sin of this type.\n";
        exit(1);
    }
}
