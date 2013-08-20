// FemSLFFEAReader.cpp: implementation of the CFemSLFFEAReader class.
//
//////////////////////////////////////////////////////////////////////

#include "FemSLFFEAReader.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFemSLFFEAReader::CFemSLFFEAReader()
{

}

CFemSLFFEAReader::~CFemSLFFEAReader()
{

}

void CFemSLFFEAReader::readFromStream(std::istream &in)
{
    int i, j;
    int ival;
    double dval;
    double dvals[8];
    char buff[255];

    CFemModel* femModel = this->getFemModel();

    if (femModel==NULL)
        return;

    CFemElementSet* elementSet = femModel->getElementSet();
    CFemNodeSet* nodeSet = femModel->getNodeSet();
    CFemMaterialSet* materialSet = femModel->getMaterialSet();
    CFemNodeBCSet* bcSet = femModel->getNodeBCSet();
    CFemNodeLoadSet* nodeLoadSet = femModel->getNodeLoadSet();
    CFemElementLoadSet* elementLoadSet = femModel->getElementLoadSet();

    nodeSet->enumerateNodes();
    materialSet->enumerateMaterials();
    elementSet->enumerateElements();
    nodeLoadSet->enumerateLoads();
    elementLoadSet->enumerateLoads();
    bcSet->enumerateBCs();

    //
    // Jump over problem information
    //

    in.getline(buff,255);
    in.getline(buff,255);
    in.getline(buff,255);
    in.getline(buff,255);

    //
    // Jump over element connectivity
    //

    in.getline(buff,255);
    for (i=0; i<elementSet->getSize(); i++)
        in.getline(buff,255);

    //
    // Jump over node coordinates
    //

    in.getline(buff,255);
    for (i=0; i<nodeSet->getSize(); i++)
        in.getline(buff,255);

    //
    // Jump over element orientation
    //

    in.getline(buff,255);
    for (i=0; i<elementSet->getSize(); i++)
        in.getline(buff,255);
    in.getline(buff,255);

    //
    // Read node displacements x
    //

    in.getline(buff,255);
    for (i=0; i<nodeSet->getSize(); i++)
    {
        in >> ival >> dval;
        CFemNode* node = nodeSet->getNode(i);
        node->setValueSize(6);
        node->setValue(0,dval);
    }
    in.getline(buff,255);
    in.getline(buff,255);

    //
    // Read node displacements y
    //

    in.getline(buff,255);
    for (i=0; i<nodeSet->getSize(); i++)
    {
        in >> ival >> dval;
        CFemNode* node = nodeSet->getNode(i);
        node->setValue(1,dval);
    }
    in.getline(buff,255);
    in.getline(buff,255);

    //
    // Read node displacements z
    //

    in.getline(buff,255);
    for (i=0; i<nodeSet->getSize(); i++)
    {
        in >> ival >> dval;
        CFemNode* node = nodeSet->getNode(i);
        node->setValue(2,dval);
    }
    in.getline(buff,255);
    in.getline(buff,255);

    //
    // Read node rotation x
    //

    in.getline(buff,255);
    for (i=0; i<nodeSet->getSize(); i++)
    {
        in >> ival >> dval;
        CFemNode* node = nodeSet->getNode(i);
        node->setValue(3,dval);
    }
    in.getline(buff,255);
    in.getline(buff,255);

    //
    // Read node rotation y
    //

    in.getline(buff,255);
    for (i=0; i<nodeSet->getSize(); i++)
    {
        in >> ival >> dval;
        CFemNode* node = nodeSet->getNode(i);
        node->setValue(4,dval);
    }
    in.getline(buff,255);
    in.getline(buff,255);

    //
    // Read node rotation z
    //

    in.getline(buff,255);
    for (i=0; i<nodeSet->getSize(); i++)
    {
        in >> ival >> dval;
        CFemNode* node = nodeSet->getNode(i);
        node->setValue(5,dval);
    }
    in.getline(buff,255);
    in.getline(buff,255);

    //
    // Jump over nodal point loads
    //

    in.getline(buff,255);
    ival = 0;
    while (ival!=-10)
    {
        in >> ival ;
        if (ival!=-10)
            in >> dval >> dval >> dval >> dval >> dval >> dval;
    }
    in.getline(buff,255);

    //
    // Jump over distributed loads
    //

    in.getline(buff,255);
    in.getline(buff,255);

    //
    // Read element stresses
    //

    in.getline(buff,255);
    for (i=0; i<elementSet->getSize(); i++)
    {
        CFemElement* element = elementSet->getElement(i);
        element->setValueSize(8);
        in >> ival >> ival;
        for (j=0; j<4; j++)
            in >> dvals[j];

        in >> ival >> ival;
        for (j=0; j<4; j++)
            in >> dvals[j+4];

        for (j=0; j<8; j++)
            element->setValue(j, dvals[j]);
    }
}

void CFemSLFFEAReader::setFileName(const char *fileName)
{
    m_fileName = fileName;
}

void CFemSLFFEAReader::load()
{
    if (m_fileName!="")
    {
        m_outputFile.open(m_fileName.c_str(),ios::in);
        if (!m_outputFile)
        {
            cout << "\tCould not open outputfile." << endl;
            return;
        }
        this->readFromStream(m_outputFile);
        m_outputFile.close();
        remove(m_fileName.c_str());
    }
}
