#include "script_plugin.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <ofutil/util_functions.h>

using namespace std;

void ScriptPlugin::loadSource()
{
    if (m_filename != "") {
        fstream f;
        f.open(m_filename, ios::in | ios::binary);

        std::string str((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

        f.close();

        m_scriptSource = str;
    }
}

void ScriptPlugin::parse()
{
    m_params.clear();
    m_intParams.clear();
    m_floatParams.clear();
    m_paramNames.clear();

    size_t pos = 0;

    size_t firstPos = m_scriptSource.find("%%", pos);
    size_t secondPos = m_scriptSource.find("%%", firstPos + 1);

    //  %%name%%
    //  ^     ^
    //  |     |
    //  |     secondPos
    //  firstPos

    while ((firstPos != string::npos) && (secondPos != string::npos)) {
        string paramStr = m_scriptSource.substr(firstPos + 2, secondPos - firstPos - 2);

        auto sep = paramStr.find(",");
        auto sep2 = paramStr.find(",", sep + 1);

        auto name = paramStr.substr(0, sep);
        auto defValue = paramStr.substr(sep + 1, sep2 - sep - 1);
        auto valueType = paramStr.substr(sep2 + 1, paramStr.length() - 1);

        m_params[name] = {defValue, defValue, valueType};

        if (valueType == "int")
            m_intParams[name] = ofutil::to_int(m_params[name][0]);

        if (valueType == "float")
            m_floatParams[name] = ofutil::to_float(m_params[name][0]);

        if (valueType == "string") {
            std::fill(m_stringParams[name].begin(), m_stringParams[name].end(), 0);
            std::copy(m_params[name][0].begin(), m_params[name][0].end(), m_stringParams[name].data());
        }

        m_paramNames.push_back(name);

        pos = secondPos + 1;

        firstPos = m_scriptSource.find("%%", pos);
        secondPos = m_scriptSource.find("%%", firstPos + 1);
    }

    m_pluginName = this->param("pluginName");
}

void ScriptPlugin::updateParams()
{
    for (auto &name : m_paramNames) {
        auto valueType = m_params[name][2];

        if (valueType == "int")
            m_params[name][0] = ofutil::to_string(m_intParams[name]);
        if (valueType == "float")
            m_params[name][0] = ofutil::to_string(m_floatParams[name]);
        // if (valueType == "string")
        //     m_params[name][0] = ofutil::to_string(m_stringParams[name]);
    }
}

ScriptPlugin::ScriptPlugin(const std::string filename) : m_filename{filename}
{
    loadSource();
    parse();
}

std::shared_ptr<ScriptPlugin> ScriptPlugin::create(const std::string filename)
{
    return std::make_shared<ScriptPlugin>(filename);
}

void ScriptPlugin::param(const std::string name, const std::string value)
{
    if (m_params.find(name) != m_params.end())
        m_params[name][0] = value;
}

const std::string ScriptPlugin::param(const std::string name)
{
    if (m_params.find(name) != m_params.end())
        return m_params[name][0];
    else
        return "";
}

const std::string ScriptPlugin::paramDefault(const std::string name)
{
    if (m_params.find(name) != m_params.end())
        return m_params[name][1];
    else
        return "";
}

const std::string ScriptPlugin::paramType(const std::string name)
{
    if (m_params.find(name) != m_params.end())
        return m_params[name][2];
    else
        return "";
}

const std::string ScriptPlugin::name()
{
    return m_pluginName;
}

float *ScriptPlugin::floatParamRef(const std::string name)
{
    if (m_floatParams.find(name) != m_floatParams.end())
        return &m_floatParams[name];
    else
        return nullptr;
}

int *ScriptPlugin::intParamRef(const std::string name)
{
    if (m_intParams.find(name) != m_intParams.end())
        return &m_intParams[name];
    else
        return nullptr;
}

char *ScriptPlugin::stringParamRef(const std::string name)
{
    if (m_stringParams.find(name) != m_stringParams.end())
        return m_stringParams[name].data();
    else
        return nullptr;
}

const std::vector<std::string> &ScriptPlugin::paramNames()
{
    return m_paramNames;
}

const std::string ScriptPlugin::source()
{
    this->updateParams();

    std::string modifiedSource = m_scriptSource;

    for (auto &p : m_params) {
        string tag = "%%" + p.first + "," + p.second[1] + "," + p.second[2] + "%%";
        auto tagPos = modifiedSource.find(tag);
        auto tagLength = tag.length();

        if (tagPos != string::npos)
            modifiedSource.replace(modifiedSource.find(tag), tag.length(), p.second[0]);
    }
    return modifiedSource;
}
