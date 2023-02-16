#pragma once

namespace vfem
{

class Preferences
{
public:
    static Preferences& instance()
    {
        static Preferences m_instance; // Guaranteed to be destroyed.
                           // Instantiated on first use.
        return m_instance;
    }

private:
    Preferences(); // Constructor? (the {} brackets) are needed here.

    bool m_showNodeNumbers;
    bool m_useSphereNodes;

public:
    Preferences(Preferences const&) = delete;
    void operator=(Preferences const&) = delete;

    bool showNodeNumbers();
    bool useSphereNodes();

    void setShowNodeNumbers(bool flag);
    void setUseSphereNodes(bool flag);
};

}
