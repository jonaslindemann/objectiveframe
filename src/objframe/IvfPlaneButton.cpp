// IvfPlaneButton.cpp: implementation of the CIvfPlaneButton class.
//
//////////////////////////////////////////////////////////////////////

#include "IvfPlaneButton.h"
#include <ivf/IvfLineSet.h>
#include <ivfimage/IvfSgiImage.h>
#include <ivfimage/IvfPngImage.h>

#ifdef HAVE_CSTRING
#include <cstring>
#else
#include <string.h>
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIvfPlaneButton::CIvfPlaneButton(int id)
{
	this->setUseName(true);
	this->setId(id);

	CIvfMaterialPtr material;
	m_normalShape = new CIvfQuadPlane();
	m_normalShape->setSize(50.0,50.0);
	material = new CIvfMaterial();
	material->setDiffuseColor(1.0f, 1.0f, 0.8f, 0.5f);
	m_normalShape->setMaterial(material);
	material = new CIvfMaterial();
	material->setDiffuseColor(1.0f, 1.0f, 1.0f, 0.5f);
	m_normalShape->setHighlightMaterial(material);
	m_normalShape->setUseName(false);
	this->addChild(m_normalShape);
	
	m_pressedShape = new CIvfQuadPlane();
	//m_pressedShape->flipVert();
	m_pressedShape->setSize(45.0,45.0);
	material = new CIvfMaterial();
	material->setDiffuseColor(0.8f, 0.8f, 0.8f, 0.5f);
	m_pressedShape->setMaterial(material);
	m_pressedShape->setHighlightMaterial(material);
	m_pressedShape->setUseName(false);
	this->addChild(m_pressedShape);

	m_disabledShape = new CIvfQuadPlane();
	//m_disabledShape->flipVert();
	m_disabledShape->setSize(50.0,50.0);
	material = new CIvfMaterial();
	material->setDiffuseColor(0.5f, 0.5f, 0.5f, 0.5f);
	m_disabledShape->setMaterial(material);
	m_disabledShape->setHighlightMaterial(material);
	m_disabledShape->setUseName(false);
	this->addChild(m_disabledShape);

	m_checkedShape = new CIvfComposite();

	m_checkedFrame = new CIvfLineSet();
	m_checkedFrame->addCoord(-25.0,  25.0, 0.0);
	m_checkedFrame->addCoord( 25.0,  25.0, 0.0);
	m_checkedFrame->addCoord( 25.0, -25.0, 0.0);
	m_checkedFrame->addCoord(-25.0, -25.0, 0.0);
	CIvfIndex* idx = new CIvfIndex();
	idx->add((long)0,(long)1);
	idx->add((long)1,(long)2);
	idx->add((long)2,(long)3);
	idx->add((long)3,(long)0);
	m_checkedFrame->addCoordIndex(idx);
	m_checkedFrame->setUseName(false);
	m_checkedShape->addChild(m_checkedFrame);
	
	m_checkedPlane = new CIvfQuadPlane();
	//m_checkedPlane->flipVert();
	m_checkedPlane->setSize(50.0,50.0);
	material = new CIvfMaterial();
	material->setDiffuseColor(0.7f, 0.0f, 0.0f, 0.5f);
	m_checkedPlane->setMaterial(material);
	m_checkedPlane->setHighlightMaterial(material);
	m_checkedPlane->setUseName(false);
	m_checkedShape->addChild(m_checkedPlane);
	m_checkedShape->setUseName(false);

	this->addChild(m_checkedShape);

	m_hint = "";
}

CIvfPlaneButton::CIvfPlaneButton(int id, const std::string& name)
{
	CIvfPngImagePtr image = new CIvfPngImage();
	image->setFileName(name);
	if (!image->read()) 
		std::cout << "Error: Could not read texture - " << name << std::endl;

	CIvfTexturePtr texture;
	texture = new CIvfTexture();
	texture->setImage(image);
	texture->setFilters(GL_LINEAR, GL_LINEAR);
	texture->setTextureMode(GL_DECAL);

	this->setUseName(true);
	this->setId(id);

	CIvfMaterialPtr material;
	m_normalShape = new CIvfQuadPlane();
	m_normalShape->setSize(50.0,50.0);
	material = new CIvfMaterial();
	material->setDiffuseColor(0.0f, 0.0f, 0.0f, 0.5f);
	m_normalShape->setMaterial(material);
	material = new CIvfMaterial();
	material->setDiffuseColor(0.0f, 0.0f, 0.0f, 0.0f);
	m_normalShape->setHighlightMaterial(material);
	m_normalShape->setUseName(false);
	m_normalShape->setTexture(texture);
	this->addChild(m_normalShape);
	
	m_pressedShape = new CIvfQuadPlane();
	m_pressedShape->setSize(45.0,45.0);
	material = new CIvfMaterial();
	material->setDiffuseColor(0.0f, 0.0f, 0.0f, 0.5f);
	m_pressedShape->setMaterial(material);
	m_pressedShape->setHighlightMaterial(material);
	m_pressedShape->setUseName(false);
	m_pressedShape->setTexture(texture);
	this->addChild(m_pressedShape);

	m_disabledShape = new CIvfQuadPlane();
	m_disabledShape->setSize(50.0,50.0);
	material = new CIvfMaterial();
	material->setDiffuseColor(0.5f, 0.5f, 0.5f, 0.5f);
	m_disabledShape->setMaterial(material);
	m_disabledShape->setHighlightMaterial(material);
	m_disabledShape->setUseName(false);
	m_disabledShape->setTexture(texture);
	this->addChild(m_disabledShape);

	m_checkedShape = new CIvfComposite();

	m_checkedFrame = new CIvfLineSet();
	m_checkedFrame->addCoord(-25.0,  25.0, 0.0);
	m_checkedFrame->addCoord( 25.0,  25.0, 0.0);
	m_checkedFrame->addCoord( 25.0, -25.0, 0.0);
	m_checkedFrame->addCoord(-25.0, -25.0, 0.0);
	m_checkedFrame->addColor(0.3f, 0.3f, 0.0f, 0.2f);
	m_checkedFrame->addColor(0.3f, 0.3f, 0.0f, 0.2f);
	m_checkedFrame->addColor(0.3f, 0.3f, 0.0f, 0.2f);
	m_checkedFrame->addColor(0.3f, 0.3f, 0.0f, 0.2f);
	
	CIvfIndexPtr idx = new CIvfIndex();
	idx->add((long)0,(long)1);
	idx->add((long)1,(long)2);
	idx->add((long)2,(long)3);
	idx->add((long)3,(long)0);
	m_checkedFrame->addCoordIndex(idx);

	CIvfIndexPtr colorIdx = new CIvfIndex();
	colorIdx->assignFrom(idx);
	m_checkedFrame->addColorIndex(idx);
	m_checkedFrame->setUseColor(true);
	m_checkedFrame->setUseAlpha(true);

	m_checkedFrame->setUseName(false);
	m_checkedShape->addChild(m_checkedFrame);
	
	m_checkedPlane = new CIvfQuadPlane();
	m_checkedPlane->setSize(50.0,50.0);
	material = new CIvfMaterial();
	material->setDiffuseColor(0.7f, 0.0f, 0.0f, 0.5f);
	m_checkedPlane->setMaterial(material);
	m_checkedPlane->setHighlightMaterial(material);
	m_checkedPlane->setUseName(false);
	m_checkedPlane->setTexture(texture);
	m_checkedShape->addChild(m_checkedPlane);
	m_checkedShape->setUseName(false);

	this->addChild(m_checkedShape);

	m_hint = "";
}

CIvfPlaneButton::~CIvfPlaneButton()
{

}

void CIvfPlaneButton::setTexture(CIvfTexture *texture)
{
	m_normalShape->setTexture(texture);
	m_pressedShape->setTexture(texture);
	m_disabledShape->setTexture(texture);
}

void CIvfPlaneButton::setSize(double width, double height)
{
	m_normalShape->setSize(width,height);
	m_normalShape->flipVert();
	m_pressedShape->setSize(width*0.95,height*0.95);
	m_pressedShape->flipVert();
	m_disabledShape->setSize(width,height);
	m_disabledShape->flipVert();
	m_checkedPlane->setSize(width,height);
	m_checkedPlane->flipVert();
	m_checkedFrame->setCoord(0, -width/2.0,  height/2.0, 0.0);
	m_checkedFrame->setCoord(1,  width/2.0,  height/2.0, 0.0);
	m_checkedFrame->setCoord(2,  width/2.0, -height/2.0, 0.0);
	m_checkedFrame->setCoord(3, -width/2.0, -height/2.0, 0.0);
}


void CIvfPlaneButton::setHint(const std::string& hintText)
{
	m_hint = hintText;
}

const std::string CIvfPlaneButton::getHint()
{
	return m_hint;
}
