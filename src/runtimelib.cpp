#include "runtimelib.h"

std::unordered_set<std::string> Azurite::builtins = {"print", "sin", "floor", "abs", "rnd"};

void Azurite::initialize_runtimelib()
{
   srand(time(NULL)); 
}

bool Azurite::has_builtin(std::string name) {
    return Azurite::builtins.count(name);
}

RuntimeValPtr Azurite::call_runtimelib(std::string name, std::vector<RuntimeValPtr>& args)
{
    if (name == "print") {
        return Azurite::print(args);
    } else if (name == "sin") {
        return Azurite::sin(args);
    } else if (name == "floor") {
        return Azurite::floor(args);
    } else if (name == "abs") {
        return Azurite::abs(args);
    } else if (name == "rnd") {
        return Azurite::rnd(args);
    }
}

RuntimeValPtr Azurite::print(std::vector<RuntimeValPtr>& args)
{
    for (RuntimeValPtr arg : args) {
        switch(arg->type) {
            case RuntimeType::String: {
                std::shared_ptr<String> arg_str = std::dynamic_pointer_cast<String>(arg);
                std::cout << arg_str->value << " ";
                break;
            }
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
    if (args[0]->type != RuntimeType::Number) {
        std::cout << "Cannot take sin of this type.\n";
        exit(1);
    }

    std::shared_ptr<Number> arg_num = std::dynamic_pointer_cast<Number>(args[0]);
    return std::make_shared<Number>(std::sin(arg_num->value));
}

RuntimeValPtr Azurite::floor(std::vector<RuntimeValPtr>& args)
{
    if (args[0]->type != RuntimeType::Number) {
        std::cout << "Cannot take floor of this type.\n";
        exit(1);
    }
    
    std::shared_ptr<Number> arg_num = std::dynamic_pointer_cast<Number>(args[0]);
    return std::make_shared<Number>(std::floor(arg_num->value));
}

RuntimeValPtr Azurite::abs(std::vector<RuntimeValPtr>& args)
{
    if (args[0]->type != RuntimeType::Number) {
        std::cout << "Cannot take floor of this type.\n";
        exit(1);
    }
    
    std::shared_ptr<Number> arg_num = std::dynamic_pointer_cast<Number>(args[0]);
    return std::make_shared<Number>(std::abs(arg_num->value));
}

RuntimeValPtr Azurite::rnd(std::vector<RuntimeValPtr>& args)
{
    double random = (float)(rand()) / ((float)(RAND_MAX) + 1);
    return std::make_shared<Number>(random);
}