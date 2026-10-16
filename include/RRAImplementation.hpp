/**
 * @file RRAImplementation.hpp
 * @brief Wrapper for the RRA tool
 *
 * @author Samuel Archambault
 * @date 2026-10-12
*/
#ifndef BIOGENIUS_SIMULATION_RRA_HPP
#define BIOGENIUS_SIMULATION_RRA_HPP

#include <OpenSim/Common/Function.h>
#include <OpenSim/Tools/RRATool.h>
#include "openSimFoldersPaths.hpp"
/**
* @brief Implements the RRA tool and serves to change a few settings
* 
* @details The constructor takes a RRA setup file as a parameter. The rest of the functions are to modify the settings given by that setup file
*/
class RRAImplementation {
	public:
		RRAImplementation(const std::string &p_setupFileName);
		~RRAImplementation();

		//These functions only exist to rename the opensim functions to make them clearer.
		//void setModelFileName(std::string modelFileName) { rraTool->setDesiredPointsFileName(modelFileName); }; //This is not the model, change it
		//void setMotionFileName(std::string motionFileName) { rraTool->setDesiredKinematicsFileName(motionFileName); };
		//void setActuatorsFileName(std::string actuatorsFileName) { rraTool->setConstraintsFileName(actuatorsFileName); };
		//void setTasksFileName(std::string tasksFileName) { rraTool->setTaskSetFileName(tasksFileName); };

	private:
		OpenSim::RRATool* m_rraTool;
};

#endif