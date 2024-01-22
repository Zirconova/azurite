#pragma once

#include <iostream>
#include <string>
#include <unordered_set>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <memory>

#include "runtimeval.h"

typedef std::shared_ptr<RuntimeVal> RuntimeValPtr;

namespace Azurite {
    extern std::unordered_set<std::string> builtins;

    void initialize_runtimelib();
    bool has_builtin(std::string name);
    RuntimeValPtr call_runtimelib(std::string name, std::vector<RuntimeValPtr>& args);

    RuntimeValPtr print(std::vector<RuntimeValPtr>& args);
    RuntimeValPtr sin(std::vector<RuntimeValPtr>& args);
    RuntimeValPtr rnd(std::vector<RuntimeValPtr>& args);
}
