#include <cmath>

#include "runtimelib.h"

std::unordered_set<std::string> Azurite::builtins = {"print", "sin"};

std::shared_ptr<RuntimeVal> Azurite::call_runtimelib(std::string name, std::vector<std::shared_ptr<RuntimeVal>>& args)
{
    if (name == "print") {
        return Azurite::print(args);
    } else if (name == "sin") {
        return Azurite::sin(args);
    }
}

std::shared_ptr<RuntimeVal> Azurite::print(std::vector<std::shared_ptr<RuntimeVal>>& args)
{
    for (std::shared_ptr<RuntimeVal> arg : args) {
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

std::shared_ptr<RuntimeVal> Azurite::sin(std::vector<std::shared_ptr<RuntimeVal>>& args)
{
    if (args[0]->type == RuntimeType::Number) {
        std::shared_ptr<Number> arg_num = std::dynamic_pointer_cast<Number>(args[0]);
        return std::make_shared<Number>(std::sin(arg_num->value));
    } else {
        std::cout << "Cannot take sin of this type.\n";
        exit(1);
    }
}
