#ifndef _CIvfArea2D_h_
#define _CIvfArea2D_h_

#include <ivf/ivfconfig.h>

#include <ivf/IvfBase.h>

#include <ivf/IvfGL.h>


namespace std {};
using namespace std;

class CIvfArea2D : public CIvfBase {
private:
	vector<int> m_xCoords;
	vector<int> m_yCoords;
	vector<float> m_red;
	vector<float> m_green;
	vector<float> m_blue;
	bool m_visible;
public:
	CIvfArea2D();
	virtual ~CIvfArea2D();

	void add(int x, int y);
	void clear();
	int getSize();

	void setCoord(int idx, int x, int y);
	void getCoord(int idx, int &x, int &y);
	void setColor(int idx, float red, float green, float blue);
	void setVisible(bool flag);

	bool inside(int x, int y);
	void render();
};

#endif 
