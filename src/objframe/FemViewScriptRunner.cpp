#include "FemViewScriptRunner.h"

#include "FemView.h"
#include "script_plugin.h"

#include <chaiscript/chaiscript.hpp>

#include <ofutil/util_functions.h>

void FemViewScriptRunner::runPlugin(FemViewWindow &view, ScriptPlugin *plugin)
{
    view.snapShot();

    chaiscript::ChaiScript script;
    view.setupScript(script);

    chaiscript::ChaiScript::State state = script.get_state();
    try
    {
        view.m_scripting.pluginRunning = true;
        script.eval(plugin->source());
    } catch (const chaiscript::exception::eval_error &e)
    {
        view.log(e.pretty_print());
    }
    view.m_scripting.pluginRunning = false;
    script.set_state(state);

    // The plugin is placed at the position picked in onSelectPosition, which
    // addNode() adds to every coordinate. Clear it again so the offset does not
    // leak into later addNode() calls (property inspector Copy, REST API, ...).

    view.m_selectedPos[0] = 0.0;
    view.m_selectedPos[1] = 0.0;
    view.m_selectedPos[2] = 0.0;

    view.m_beamModel->enumerate();
}

void FemViewScriptRunner::runScript(FemViewWindow &view, const std::string &scriptFilename)
{
    view.snapShot();

    view.m_selectedPos[0] = 0.0;
    view.m_selectedPos[1] = 0.0;
    view.m_selectedPos[2] = 0.0;

    chaiscript::ChaiScript chai;
    view.setupScript(chai);

    auto scriptSource = ofutil::read_file(scriptFilename);

    try
    {
        chai.eval(scriptSource);
    } catch (const chaiscript::exception::eval_error &e)
    {
        view.log(e.pretty_print());
    }

    view.m_beamModel->enumerate();

    view.set_changed();
    view.redraw();
}

void FemViewScriptRunner::runScriptFromText(FemViewWindow &view, const std::string &scriptText)
{
    view.m_beamModel->enumerate();

    view.snapShot();

    view.m_selectedPos[0] = 0.0;
    view.m_selectedPos[1] = 0.0;
    view.m_selectedPos[2] = 0.0;

    chaiscript::ChaiScript chai;
    view.setupScript(chai);

    view.m_scripting.running = true;
    try
    {
        chai.eval(scriptText);
    } catch (const chaiscript::exception::eval_error &e)
    {
        view.m_promptWindow->clearError();
        view.m_promptWindow->addError(e.pretty_print());
        view.log(e.pretty_print());
    }
    view.m_scripting.running = false;

    view.m_beamModel->enumerate();

    view.set_changed();
    view.redraw();
}
