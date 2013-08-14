// Implementation of: public class CFemModel

#include "FemModel.h"

// ------------------------------------------------------------
CFemModel::CFemModel ()
:CFemObject()
{
	m_nodeSet = NULL;
	m_elementSet = NULL;
	m_materialSet = NULL;
	m_fileName = "";
	m_nodeLoadSet = NULL;
	m_elementLoadSet = NULL;
	m_bcSet = NULL;
}

// ------------------------------------------------------------
CFemModel::~CFemModel ()
{
	if (m_nodeSet!=NULL)
		delete m_nodeSet;
	if (m_elementSet!=NULL)
		delete m_elementSet;
	if (m_materialSet!=NULL)
		delete m_materialSet;
	if (m_nodeLoadSet!=NULL)
		delete m_nodeLoadSet;
	if (m_elementLoadSet!=NULL)
		delete m_elementLoadSet;
	if (m_bcSet!=NULL)
		delete m_bcSet;
}

// ------------------------------------------------------------
CFemNodeSet* CFemModel::getNodeSet()
{
	return m_nodeSet;
}

// ------------------------------------------------------------
CFemElementSet* CFemModel::getElementSet()
{
	return m_elementSet;
}

// ------------------------------------------------------------
CFemMaterialSet* CFemModel::getMaterialSet()
{
	return m_materialSet;
}

// ------------------------------------------------------------
CFemNodeLoadSet* CFemModel::getNodeLoadSet()
{
	return m_nodeLoadSet;
}

// ------------------------------------------------------------
CFemElementLoadSet* CFemModel::getElementLoadSet()
{
	return m_elementLoadSet;
}

// ------------------------------------------------------------
CFemNodeBCSet* CFemModel::getBCSet()
{
	return m_bcSet;
}

// ------------------------------------------------------------
CFemNodeSet* CFemModel::createNodeSet()
{
	return new CFemNodeSet();
}

// ------------------------------------------------------------
CFemElementSet* CFemModel::createElementSet()
{
	return new CFemElementSet();
}

// ------------------------------------------------------------
CFemMaterialSet* CFemModel::createMaterialSet()
{
	return new CFemMaterialSet();
}

// ------------------------------------------------------------
CFemNodeLoadSet* CFemModel::createNodeLoadSet()
{
	return new CFemNodeLoadSet();
}

// ------------------------------------------------------------
CFemElementLoadSet* CFemModel::createElementLoadSet()
{
	return new CFemElementLoadSet();
}

// ------------------------------------------------------------
CFemNodeBCSet* CFemModel::createBCSet()
{
	return new CFemNodeBCSet();
}

// ------------------------------------------------------------
void CFemModel::print(ostream &out)
{
	m_materialSet->print(out);
	m_nodeSet->print(out);
	m_elementSet->print(out);
}

// ------------------------------------------------------------
void CFemModel::saveToStream(std::ostream &out)
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
void CFemModel::readFromStream(std::istream &in)
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
void CFemModel::connectElements()
{
	m_elementSet->connectNodes(m_nodeSet);
}

// ------------------------------------------------------------
void CFemModel::connectMaterials()
{

}

// ------------------------------------------------------------
void CFemModel::connectNodeLoads()
{
	m_nodeLoadSet->connectNodes(m_nodeSet);
}

// ------------------------------------------------------------
void CFemModel::connectElementLoads()
{
	m_elementLoadSet->connectElements(m_elementSet);
}

// ------------------------------------------------------------
void CFemModel::connectNodeBCs()
{
	m_bcSet->connectNodes(m_nodeSet);
}

// ------------------------------------------------------------
void CFemModel::deleteAll()
{
	m_materialSet->deleteAll();
	m_elementSet->deleteAll();
	m_nodeSet->deleteAll();
	m_nodeLoadSet->deleteAll();
	m_elementLoadSet->deleteAll();
	m_bcSet->deleteAll();
}

// ------------------------------------------------------------
void CFemModel::setFileName(const std::string& fname)
{
	m_fileName = fname;
}

// ------------------------------------------------------------
const std::string CFemModel::getFileName()
{
	return m_fileName;
}

// ------------------------------------------------------------
void CFemModel::open()
{
	if (m_fileName!="")
	{
		fstream inputFile;
		inputFile.open(m_fileName.c_str(), ios::in);
		this->readFromStream(inputFile);
		inputFile.close();
	}
}

// ------------------------------------------------------------
void CFemModel::save()
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
void CFemModel::initialize()
{
	// If existing structures exits delete them 

	if (m_nodeSet!=NULL)
		delete m_nodeSet;
	if (m_elementSet!=NULL)
		delete m_elementSet;
	if (m_materialSet!=NULL)
		delete m_materialSet;
	if (m_nodeLoadSet!=NULL)
		delete m_nodeLoadSet;
	if (m_elementLoadSet!=NULL)
		delete m_elementLoadSet;
	if (m_bcSet!=NULL)
		delete m_bcSet;

	// Create new sets
	
	m_elementLoadSet = this->createElementLoadSet();
	m_nodeSet = this->createNodeSet();
	m_elementSet = this->createElementSet();
	m_materialSet = this->createMaterialSet();
	m_nodeLoadSet = this->createNodeLoadSet();
	m_bcSet = this->createBCSet();
}

CFemNodeBCSet* CFemModel::getNodeBCSet()
{
	return m_bcSet;
}
