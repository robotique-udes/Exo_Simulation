#include "RRAImplementation.hpp"

RRAImplementation::RRAImplementation(const std::string &p_setupFileName)
{
	m_rraTool = new OpenSim::RRATool(p_setupFileName);
}

RRAImplementation::~RRAImplementation() 
{
	delete m_rraTool;
}
