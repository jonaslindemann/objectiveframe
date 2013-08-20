#ifndef _CFemSLFFEAReader_h_
#define _CFemSLFFEAReader_h_

#include "FemOutputFileReader.h"

#include <string>

FemSmartPointer(CFemSLFFEAReader);

class CFemSLFFEAReader : public CFemOutputFileReader {
private:
    string m_fileName;
    fstream m_outputFile;
public:
    CFemSLFFEAReader();
    virtual ~CFemSLFFEAReader();

    FemClassInfo("CFemSLFFEAReader",CFemOutputFileReader);

    void setFileName(const char* fileName);
    void load();


    virtual void readFromStream(istream &in);
};

#endif // !defined(AFX_FEMSLFFEAREADER_H__F276BD4E_3DDB_11D4_94EE_004005A46C57__INCLUDED_)
