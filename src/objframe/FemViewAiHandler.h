#pragma once

#include <string>

class FemViewWindow;

class FemViewAiHandler {
public:
    static void makeRequest(FemViewWindow &view, const std::string &userPrompt);
    static void onGenerationComplete(FemViewWindow &view, const std::string &result, bool success);
};
