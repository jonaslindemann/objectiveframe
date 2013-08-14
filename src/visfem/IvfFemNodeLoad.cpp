// Implementation of: public class CIvfFemNodeLoad

#include "IvfFemNodeLoad.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_CMATH
#include <cmath>
#else
#include <math.h>
#endif

namespace std {};
using namespace std;

// ------------------------------------------------------------
CIvfFemNodeLoad::CIvfFemNodeLoad ()
		:CIvfShape()
{
	m_beamModel = NULL;
	m_nodeLoad = NULL;

	m_arrowMaterial = new CIvfMaterial();
	m_arrowMaterial->setAmbientColor(0.3f, 0.0f, 0.0f, 1.0f);
	m_arrowMaterial->setDiffuseColor(1.0f, 0.0f, 0.0f, 1.0f);
	m_arrowMaterial->setSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	this->setMaterial(m_arrowMaterial);
	this->setUseName(true);
	this->setUseSelectShape(false);
}

// ------------------------------------------------------------
CIvfFemNodeLoad::~CIvfFemNodeLoad ()
{
}

// ------------------------------------------------------------
void CIvfFemNodeLoad::doCreateGeometry()
{
	int i;
	for (i=0; i<m_arrow.size(); i++)
		m_arrow[i]->render();
}

// ------------------------------------------------------------
void CIvfFemNodeLoad::doCreateSelect()
{

}

// ------------------------------------------------------------
void CIvfFemNodeLoad::initArrow()
{
	double x, y, z, ex, ey, ez;
	double loadHeight;
	double l;

	if (m_nodeLoad!=NULL)
	{

		for (int i=0; i<m_arrow.size(); i++)
		{
			if (m_beamModel!=NULL)
				loadHeight = m_beamModel->getLoadSize();
			else
				loadHeight = 1.0;
			
			// Set topolgy

			CFemNode* node = m_nodeLoad->getNode(i);
			
			// Define arrow
			
			m_arrow[i]->setSize(loadHeight*0.6, loadHeight*0.6*0.20);
			m_arrow[i]->setRadius(loadHeight*0.05, loadHeight*0.03);
			
			// Calculate arrow position
			
			node->getCoord(x, y, z);

			// Calculate arrow direction

			m_nodeLoad->getDirection(ex, ey, ez);

			l = sqrt(pow(ex,2) + pow(ey,2) + pow(ez,2));
			ex = ex/l;
			ey = ey/l;
			ez = ez/l;

			double sign = 1.0;
			if (m_nodeLoad->getValue()<0)
				sign = -1.0;

			m_arrow[i]->setDirection(sign*ex, sign*ey, sign*ez);
			m_arrow[i]->setPosition(
				x - sign*0.6*loadHeight*ex, 
				y - sign*0.6*loadHeight*ey, 
				z - sign*0.6*loadHeight*ez);
			m_arrow[i]->refresh();
			if (m_beamModel!=NULL)
				m_beamModel->getColorTable()->assignColor(m_nodeLoad->getColor(), m_arrowMaterial);
		}
	}
}

// ------------------------------------------------------------
void CIvfFemNodeLoad::refresh()
{
	setNodeLoad(m_nodeLoad);
	initArrow();
}

// ------------------------------------------------------------
void CIvfFemNodeLoad::setNodeLoad(CFemBeamNodeLoad *nodeLoad)
{
	int i;

	for (i=0; i<m_arrow.size(); i++)
	{
		delete m_arrow[i];
	}
	m_arrow.clear();

	m_nodeLoad = nodeLoad;

	if (m_nodeLoad!=NULL)
	{
		for (i=0; i<m_nodeLoad->getNodeSize(); i++)
		{
			CIvfExtrArrow* arrow = new CIvfExtrArrow();
			m_arrow.push_back(arrow);
			arrow->setMaterial(m_arrowMaterial);
		}
	}
}

void CIvfFemNodeLoad::setBeamModel(CIvfBeamModel *model)
{
	m_beamModel = model;
}
