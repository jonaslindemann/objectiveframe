#include <ofem/model_state_info.h>

ofem::ModelStateInfo::ModelStateInfo::ModelStateInfo() : m_version{"1"}, m_readVersion{""}, m_writeVersion{""}
{
}

void ofem::ModelStateInfo::setVersion(std::string version)
{
    m_version = version;
}

std::string ofem::ModelStateInfo::version()
{
    return m_version;
}

void ofem::ModelStateInfo::setReadVersion(std::string version)
{
    m_readVersion = version;
}

std::string ofem::ModelStateInfo::readVersion()
{
    return m_readVersion;
}

void ofem::ModelStateInfo::setWriteVersion(std::string version)
{
    m_writeVersion = version;
}

std::string ofem::ModelStateInfo::writeVersion()
{
    return m_writeVersion;
}
