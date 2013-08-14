// Implementation of: public class CFemDof

#include "FemDof.h"

// ------------------------------------------------------------
CFemDof::CFemDof ()
		:CFemObject()
{
	m_kind = FEM_DISPL_X;
	m_number = -1;
}

// ------------------------------------------------------------
CFemDof::CFemDof (int kind, int number)
		:CFemObject()
{
	m_kind = kind;
	m_number = number;
}

// ------------------------------------------------------------
CFemDof::~CFemDof ()
{
}

// ------------------------------------------------------------
void CFemDof::print(std::ostream &out)
{
	out << m_kind+1 << " ";
}

// ------------------------------------------------------------
void CFemDof::setKind(int kind)
{
	m_kind = kind;
}

// ------------------------------------------------------------
int CFemDof::getKind()
{
	return m_kind;
}

// ------------------------------------------------------------
void CFemDof::setNumber(long number)
{
	m_number = number;
}

// ------------------------------------------------------------
long CFemDof::getNumber()
{
	return m_number;
}
