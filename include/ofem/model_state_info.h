#pragma once

#include <string>

namespace ofem
{

    class ModelStateInfo
{
public:
    static ModelStateInfo& getInstance()
    {
        static ModelStateInfo instance; // Guaranteed to be destroyed.
                                        // Instantiated on first use.
        return instance;
    }

private:
    ModelStateInfo(); // Constructor? (the {} brackets) are needed here.

    std::string m_version;
    std::string m_readVersion;
    std::string m_writeVersion;

    // C++ 11
    // =======
    // We can use the better technique of deleting the methods
    // we don't want.
public:
    ModelStateInfo(ModelStateInfo const&) = delete;
    void operator=(ModelStateInfo const&) = delete;

    void setVersion(std::string version);
    std::string version();

    void setReadVersion(std::string version);
    std::string readVersion();

    void setWriteVersion(std::string version);
    std::string writeVersion();

    // Note: Scott Meyers mentions in his Effective Modern
    //       C++ book, that deleted functions should generally
    //       be public as it results in better error messages
    //       due to the compilers behavior to check accessibility
    //       before deleted status
};

}
