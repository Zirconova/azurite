#pragma once

#include <unordered_set>

#include "runtimeval.h"

typedef std::shared_ptr<RuntimeVal> RuntimeValPtr;

namespace Azurite {
    extern std::unordered_set<std::string> builtins;

    RuntimeValPtr call_runtimelib(std::string name, std::vector<RuntimeValPtr>& args);

    RuntimeValPtr print(std::vector<RuntimeValPtr>& args);
    RuntimeValPtr sin(std::vector<RuntimeValPtr>& args);
}