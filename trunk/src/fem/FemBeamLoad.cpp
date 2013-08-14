// Implementation of: public class CFemBeamLoad

#include "FemBeamLoad.h"

// ------------------------------------------------------------
CFemBeamLoad::CFemBeamLoad ()
:CFemElementLoad()
{
	m_name = "Noname";
	m_color = 1;
}

// ------------------------------------------------------------
CFemBeamLoad::~CFemBeamLoad ()
{
}

// ------------------------------------------------------------
void CFemBeamLoad::setName(const std::string& name)
{
	m_name = name;
}

// ------------------------------------------------------------
const std::string CFemBeamLoad::getName()
{
	return m_name;
}

// ------------------------------------------------------------
void CFemBeamLoad::saveToStream(std::ostream &out)
{
	CFemElementLoad::saveToStream(out);
	if (m_name=="")
		out << "Noname" << endl;
	else
		out << m_name << endl;
	out << m_color << endl;
}

// ------------------------------------------------------------
void CFemBeamLoad::readFromStream(std::istream &in)
{
	char buffer[255];
	CFemElementLoad::readFromStream(in);
	in.getline(buffer,255);
	in.getline(buffer,255);
	this->setName(buffer);
	in >> m_color;
}

// ------------------------------------------------------------
void CFemBeamLoad::setColor(int color)
{
	m_color = color;
}

// ------------------------------------------------------------
int CFemBeamLoad::getColor()
{
	return m_color;
}

