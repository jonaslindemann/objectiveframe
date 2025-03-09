#pragma once

#include <string>
#include <map>

namespace ofutil {

class AppSettings {
public:
    static AppSettings *instance();

    void setAppName(const std::string &appName);
    std::string appName() const;

    void set(const std::string &key, const std::string &value);
    std::string get(const std::string &key) const;

    void save();
    void load();

private:
    AppSettings();
    AppSettings(const AppSettings &);
    AppSettings &operator=(const AppSettings &)
    {
        return *this;
    };

    static AppSettings *m_this;



    std::string m_appName;

    std::map<std::string, std::string> m_settings;
};

} // namespace ofutil
