#ifndef _CStructureFactory_h_
#define _CStructureFactory_h_

#include <ivf/IvfScene.h>

#include <IvfBeamModel.h>
#include <IvfFemNode.h>
#include <IvfFemBeam.h>

class CStructureFactory {
private:
	int m_size[3];
	double m_spacing[3];
	CIvfBeamModel* m_beamModel;
	CFemBeamMaterial* m_currentMaterial;
public:
	void setCurrentMaterial(CFemBeamMaterial* material);
	CStructureFactory();
	virtual ~CStructureFactory();

	// Methods

	void create();

	// Get/set methods

	void setSize(int rows, int cols, int stacks);
	void getSize(int &rows, int &cols, int &stacks);
	void setSpacing(double xSpacing, double ySpacing, double zSpacing);
	void getSpacing(double &xSpacing, double &ySpacing, double &zSpacing);
	void setBeamModel(CIvfBeamModel* model);
	CIvfBeamModel* getBeamModel();
};

#endif 
