#pragma once

#include <array>
#include <map>
#include <memory>
#include <string>
#include <vector>

class ScriptPlugin {
private:
    std::string m_filename;
    std::string m_scriptSource;
    std::string m_pluginName;

    std::map<std::string, std::vector<std::string>> m_params;
    std::vector<std::string> m_paramNames;

    std::map<std::string, int> m_intParams;
    std::map<std::string, float> m_floatParams;
    std::map<std::string, std::array<char, 255>> m_stringParams;

    void loadSource();
    void parse();
    void updateParams();

public:
    ScriptPlugin(const std::string filename);

    static std::shared_ptr<ScriptPlugin> create(const std::string filename);

    void param(const std::string name, const std::string value);
    const std::string param(const std::string name);
    const std::string paramDefault(const std::string name);
    const std::string paramType(const std::string name);
    const std::string name();

    float *floatParamRef(const std::string name);
    int *intParamRef(const std::string name);
    char *stringParamRef(const std::string name);

    const std::vector<std::string> &paramNames();

    const std::string source();
};

typedef std::shared_ptr<ScriptPlugin> ScriptPluginPtr;
