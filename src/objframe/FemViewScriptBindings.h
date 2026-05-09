#pragma once

#include <chaiscript/chaiscript.hpp>

class FemViewWindow;

class FemViewScriptBindings {
public:
    static void bind(chaiscript::ChaiScript &script, FemViewWindow &view);
};
