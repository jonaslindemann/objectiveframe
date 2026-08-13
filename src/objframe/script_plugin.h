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
    std::string m_pluginCategory;

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

    // Parameters that describe the plugin itself rather than its input. These
    // are read by the application and must not be shown as editable fields.

    static bool isReservedParam(const std::string &name);

    void param(const std::string name, const std::string value);
    const std::string param(const std::string name);
    const std::string paramDefault(const std::string name);
    const std::string paramType(const std::string name);
    const std::string name();

    // Empty for plugins that declare no pluginCategory tag. Those are listed
    // directly in the Create menu instead of in a submenu.

    const std::string category();

    float *floatParamRef(const std::string name);
    int *intParamRef(const std::string name);
    char *stringParamRef(const std::string name);

    const std::vector<std::string> &paramNames();

    const std::string source();
};

typedef std::shared_ptr<ScriptPlugin> ScriptPluginPtr;
