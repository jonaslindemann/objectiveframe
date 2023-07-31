#include "preferences.h"

vfem::Preferences::Preferences() : m_showNodeNumbers{true}, m_useSphereNodes{true}
{
}

bool vfem::Preferences::showNodeNumbers()
{
    return m_showNodeNumbers;
}

bool vfem::Preferences::useSphereNodes()
{
    return m_useSphereNodes;
}

void vfem::Preferences::setShowNodeNumbers(bool flag)
{
    m_showNodeNumbers = flag;
}

void vfem::Preferences::setUseSphereNodes(bool flag)
{
    m_useSphereNodes = flag;
}
