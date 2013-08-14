#include "FemBeamNodeLoad.h"

// ------------------------------------------------------------
CFemBeamNodeLoad::CFemBeamNodeLoad()
:CFemNodeLoad()
{
	m_name = "Noname";
	m_color = 1;
}

// ------------------------------------------------------------
CFemBeamNodeLoad::~CFemBeamNodeLoad()
{
}

// ------------------------------------------------------------
void CFemBeamNodeLoad::setName(const std::string& name)
{
	m_name = name;
}

// ------------------------------------------------------------
const std::string CFemBeamNodeLoad::getName()
{
	return m_name;
}

// ------------------------------------------------------------
void CFemBeamNodeLoad::saveToStream(std::ostream &out)
{
	CFemNodeLoad::saveToStream(out);
	if (m_name=="")
		out << "Noname" << endl;
	else
		out << m_name << endl;
	out << m_color << endl;		
}

// ------------------------------------------------------------
void CFemBeamNodeLoad::readFromStream(std::istream &in)
{
	char buffer[255];
	CFemNodeLoad::readFromStream(in);
	in.getline(buffer,255);
	in.getline(buffer,255);
	this->setName(buffer);
	in >> m_color;
}

// ------------------------------------------------------------
void CFemBeamNodeLoad::setColor(int color)
{
	m_color = color;
}

// ------------------------------------------------------------
int CFemBeamNodeLoad::getColor()
{
	return m_color;
}
