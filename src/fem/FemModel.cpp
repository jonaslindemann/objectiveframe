// Implementation of: public class CFemModel

#include "FemModel.h"

// ------------------------------------------------------------
FemModel::FemModel ()
    :FemObject()
{
}

// ------------------------------------------------------------
FemModel::~FemModel ()
{
}

// ------------------------------------------------------------
FemNodeSet* FemModel::getNodeSet()
{
    return m_nodeSet;
}

FemNodeSet* FemModel::nodeSet()
{
	return m_nodeSet;
}

// ------------------------------------------------------------
FemElementSet* FemModel::getElementSet()
{
	return m_elementSet;
}

FemElementSet* FemModel::elementSet()
{
	return m_elementSet;
}

// ------------------------------------------------------------
FemMaterialSet* FemModel::getMaterialSet()
{
    return m_materialSet;
}

FemMaterialSet* FemModel::materialSet()
{
	return m_materialSet;
}

// ------------------------------------------------------------
FemNodeLoadSet* FemModel::getNodeLoadSet()
{
    return m_nodeLoadSet;
}

FemNodeLoadSet* FemModel::nodeLoadSet()
{
	return m_nodeLoadSet;
}

// ------------------------------------------------------------
FemElementLoadSet* FemModel::getElementLoadSet()
{
    return m_elementLoadSet;
}

FemElementLoadSet* FemModel::elementLoadSet()
{
	return m_elementLoadSet;
}

// ------------------------------------------------------------
FemNodeBCSet* FemModel::getBCSet()
{
    return m_bcSet;
}

FemNodeBCSet* FemModel::BCSet()
{
	return m_bcSet;
}

// ------------------------------------------------------------
FemNodeSet* FemModel::createNodeSet()
{
    return new FemNodeSet();
}

// ------------------------------------------------------------
FemElementSet* FemModel::createElementSet()
{
    return new FemElementSet();
}

// ------------------------------------------------------------
FemMaterialSet* FemModel::createMaterialSet()
{
    return new FemMaterialSet();
}

// ------------------------------------------------------------
FemNodeLoadSet* FemModel::createNodeLoadSet()
{
    return new FemNodeLoadSet();
}

// ------------------------------------------------------------
FemElementLoadSet* FemModel::createElementLoadSet()
{
    return new FemElementLoadSet();
}

// ------------------------------------------------------------
FemNodeBCSet* FemModel::createBCSet()
{
    return new FemNodeBCSet();
}

// ------------------------------------------------------------
void FemModel::print(ostream &out)
{
    m_materialSet->print(out);
    m_nodeSet->print(out);
    m_elementSet->print(out);
}

// ------------------------------------------------------------
void FemModel::saveToStream(std::ostream &out)
{
    // Enumerate model

    m_materialSet->enumerateMaterials();
    m_nodeSet->enumerateNodes();
    m_nodeSet->enumerateDofs();
    m_elementSet->enumerateElements();
    m_nodeLoadSet->enumerateLoads();
    m_elementLoadSet->enumerateLoads();
    m_bcSet->enumerateBCs();

    // Save to stream

    m_materialSet->saveToStream(out);
    out << endl << endl;
    m_nodeSet->saveToStream(out);
    out << endl << endl;
    m_elementSet->saveToStream(out);
    out << endl << endl;
    m_nodeLoadSet->saveToStream(out);
    out << endl << endl;
    m_elementLoadSet->saveToStream(out);
    out << endl << endl;
    m_bcSet->saveToStream(out);
}

// ------------------------------------------------------------
void FemModel::readFromStream(std::istream &in)
{
    initialize();
    deleteAll();
    m_materialSet->readFromStream(in);
    m_nodeSet->readFromStream(in);
    m_elementSet->readFromStream(in);
    m_nodeLoadSet->readFromStream(in);
    m_elementLoadSet->readFromStream(in);
    m_bcSet->readFromStream(in);
    this->connectElements();
    this->connectMaterials();
    this->connectNodeLoads();
    this->connectElementLoads();
    this->connectNodeBCs();
}

// ------------------------------------------------------------
void FemModel::connectElements()
{
    m_elementSet->connectNodes(m_nodeSet);
}

// ------------------------------------------------------------
void FemModel::connectMaterials()
{

}

// ------------------------------------------------------------
void FemModel::connectNodeLoads()
{
    m_nodeLoadSet->connectNodes(m_nodeSet);
}

// ------------------------------------------------------------
void FemModel::connectElementLoads()
{
    m_elementLoadSet->connectElements(m_elementSet);
}

// ------------------------------------------------------------
void FemModel::connectNodeBCs()
{
    m_bcSet->connectNodes(m_nodeSet);
}

// ------------------------------------------------------------
void FemModel::deleteAll()
{
    m_materialSet->deleteAll();
    m_elementSet->deleteAll();
    m_nodeSet->deleteAll();
    m_nodeLoadSet->deleteAll();
    m_elementLoadSet->deleteAll();
    m_bcSet->deleteAll();
}

// ------------------------------------------------------------
void FemModel::setFileName(const std::string& fname)
{
    m_fileName = fname;
}

// ------------------------------------------------------------
const std::string FemModel::getFileName()
{
    return m_fileName;
}

// ------------------------------------------------------------
bool FemModel::open()
{
	if (m_fileName != "")
	{
		fstream inputFile;
		inputFile.open(m_fileName.c_str(), ios::in);
		if (inputFile.is_open())
		{
			this->readFromStream(inputFile);
		}
		else
			return false;

		inputFile.close();
		return true;
	}
	else
		return false;
}

// ------------------------------------------------------------
void FemModel::save()
{
    if (m_fileName!="")
    {
        fstream outputFile;
        outputFile.open(m_fileName.c_str(),ios::out);
        this->saveToStream(outputFile);
        outputFile.close();
    }
}

// ------------------------------------------------------------
void FemModel::initialize()
{
    // Create new sets

    m_elementLoadSet = this->createElementLoadSet();
    m_nodeSet = this->createNodeSet();
    m_elementSet = this->createElementSet();
    m_materialSet = this->createMaterialSet();
    m_nodeLoadSet = this->createNodeLoadSet();
    m_bcSet = this->createBCSet();
    
    this->onInitialised();
}

// ------------------------------------------------------------
void FemModel::onInitialised()
{
    
}

// ------------------------------------------------------------
void FemModel::clearNodeValues()
{
    m_nodeSet->clearNodeValues();
}


// ------------------------------------------------------------
FemNodeBCSet* FemModel::getNodeBCSet()
{
    return m_bcSet;
}

FemNodeBCSet* FemModel::nodeBCSet()
{
	return m_bcSet;
}

