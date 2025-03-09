#include <ofutil/app_settings.h>

#include <nlohmann/json.hpp>

#include <fstream>

using namespace ofutil;

AppSettings *AppSettings::m_this = nullptr;

AppSettings *ofutil::AppSettings::instance()
{
    if (m_this == nullptr)
    {
        m_this = new AppSettings();
    }
    return m_this;
}

void ofutil::AppSettings::setAppName(const std::string &appName)
{
    m_appName = appName;
}

std::string ofutil::AppSettings::appName() const
{
    return m_appName;
}

void ofutil::AppSettings::set(const std::string &key, const std::string &value)
{
    m_settings[key] = value;
}

std::string ofutil::AppSettings::get(const std::string &key) const
{
    auto it = m_settings.find(key);
    if (it != m_settings.end())
    {
        return it->second;
    }
    return "";
}

void ofutil::AppSettings::save()
{
    nlohmann::json j;
    for (auto &it : m_settings)
    {
        j[it.first] = it.second;
    }
    std::ofstream file(m_appName + ".json");
    file << j.dump(4);
    file.close();
}

void ofutil::AppSettings::load()
{
    std::ifstream file(m_appName + ".json");
    if (file.is_open())
    {
        nlohmann::json j;
        file >> j;
        for (auto it = j.begin(); it != j.end(); ++it)
        {
            m_settings[it.key()] = it.value();
        }
        file.close();
    }
}

ofutil::AppSettings::AppSettings() : m_appName("app_settings")
{}

ofutil::AppSettings::AppSettings(const AppSettings &)
{}
