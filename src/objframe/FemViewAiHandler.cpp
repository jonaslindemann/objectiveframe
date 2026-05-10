#include "FemViewAiHandler.h"

#include "FemView.h"

void FemViewAiHandler::makeRequest(FemViewWindow &view, const std::string &userPrompt)
{
    if (view.m_ai.apiKey.empty())
    {
        view.log("No API key set for AI service.");
        return;
    }

    view.m_ai.structureGenerator.setApiKey(view.m_ai.apiKey);
    view.m_ai.structureGenerator.generateStructureAsync(
        userPrompt,
        std::bind(&FemViewWindow::onGenerationComplete, &view, std::placeholders::_1, std::placeholders::_2));

    view.m_ai.isProcessing = true;
}

void FemViewAiHandler::onGenerationComplete(FemViewWindow &view, const std::string &result, bool success)
{
    if (success)
    {
        view.log("AI generation successful.");
        view.m_promptWindow->clearOutput();
        view.m_promptWindow->addOutput(result);

        if (view.m_ai.autoRunScript)
        {
            std::lock_guard<std::mutex> lock(view.m_ai.scriptQueueMutex);
            view.m_ai.pendingScripts.push(result);
            view.log("Script queued for execution on main thread.");
        }
        view.m_ai.isProcessing = false;
    }
    else
    {
        view.log("AI generation failed.");
        view.m_ai.isProcessing = false;
    }
}
