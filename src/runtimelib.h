#pragma once

#include <unordered_set>

#include "runtimeval.h"

namespace Azurite {
    extern std::unordered_set<std::string> builtins;

    std::shared_ptr<RuntimeVal> call_runtimelib(std::string name, std::vector<std::shared_ptr<RuntimeVal>>& args);

    std::shared_ptr<RuntimeVal> print(std::vector<std::shared_ptr<RuntimeVal>>& args);
    std::shared_ptr<RuntimeVal> sin(std::vector<std::shared_ptr<RuntimeVal>>& args);
}