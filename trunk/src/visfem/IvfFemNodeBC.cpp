// Implementation of: public class CIvfFemNodeBC

#include "IvfFemNodeBC.h"

#include <ivf/IvfCone.h>
#include <ivf/IvfTransform.h>

// ------------------------------------------------------------
CIvfFemNodeBC::CIvfFemNodeBC ()
{
	this->setUseSelectShape(false);

	m_bcMaterial = new CIvfMaterial();
	m_bcMaterial->setDiffuseColor(1.0, 1.0, 1.0, 1.0);
	m_bcMaterial->addReference();

	m_nodeBC = NULL;

	m_beamModel = NULL;
}

// ------------------------------------------------------------
CIvfFemNodeBC::~CIvfFemNodeBC ()
{
	delete m_bcMaterial;
}

// ------------------------------------------------------------
void CIvfFemNodeBC::setNodeBC(CFemBeamNodeBC *bc)
{
	int i, j;

	for (i=0; i<m_bc.size(); i++)
	{
		delete m_bc[i];
	}
	m_bc.clear();

	m_nodeBC = bc;
	if (m_nodeBC!=NULL)
	{
		for (i=0; i<m_nodeBC->getNodeSize(); i++)
		{
			double x, y, z;
			CIvfBC* ivfBC = new CIvfBC();
			if (m_beamModel!=NULL)
				ivfBC->setBeamModel(m_beamModel);
			for (j=0; j<6; j++)
			{
				if (m_nodeBC->isPrescribed(j+1))
					ivfBC->prescribe(j+1,true);
				else
					ivfBC->prescribe(j+1,false);
			}
			m_bc.push_back(ivfBC);
			ivfBC->setMaterial(m_bcMaterial);
			m_nodeBC->getNode(i)->getCoord(x, y, z);
			ivfBC->setPosition(x, y, z);
			if (m_beamModel!=NULL)
				m_beamModel->getColorTable()->assignColor(m_nodeBC->getColor(), m_bcMaterial);
		}
	}
}

// ------------------------------------------------------------
void CIvfFemNodeBC::doCreateGeometry()
{
	int i;

	for (i=0; i<m_bc.size(); i++)
		m_bc[i]->render();
}

// ------------------------------------------------------------
void CIvfFemNodeBC::refresh()
{
	setNodeBC(m_nodeBC);
}

void CIvfFemNodeBC::setBeamModel(CIvfBeamModel *model)
{
	m_beamModel = model;
}
