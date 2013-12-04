#ifndef _CIvfColorTable_
#define _CIvfColorTable_

#include <ivf/IvfBase.h>
#include <ivf/IvfMaterial.h>

IvfSmartPointer(CIvfColorTable);

class CIvfColorTable : public CIvfBase
{
private:
    float m_colorTable[256][3];
public:
    CIvfColorTable();
    virtual ~CIvfColorTable();

    IvfClassInfo("CIvfColorTable",CIvfBase);

    void setColor(int idx, float r, float g, float b);
    void assignColor(int idx, CIvfMaterial* material);
    void getColor(int idx, float &r, float &g, float &b);
};

#endif
