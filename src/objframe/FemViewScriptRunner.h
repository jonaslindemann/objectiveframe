#pragma once

#include <string>

class FemViewWindow;
class ScriptPlugin;

class FemViewScriptRunner {
public:
    static void runPlugin(FemViewWindow &view, ScriptPlugin *plugin);
    static void runScript(FemViewWindow &view, const std::string &scriptFilename);
    static void runScriptFromText(FemViewWindow &view, const std::string &scriptText);
};
