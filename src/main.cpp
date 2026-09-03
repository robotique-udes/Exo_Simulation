/**
 * @file main.cpp
 * @brief Entry point of the simulation. Lists each step of the simulation
 *
 * @author Samuel Savaria
 * @date 2026-08-21
*/
#include "ModelBuilder.hpp"
#include <filesystem>
#include <iostream>
#include <format>

int main()
{
	try
	{
		std::filesystem::create_directory("Results");
		OpenSim::ModelVisualizer::addDirToGeometrySearchPaths("Geometry");
		OpenSim::ModelVisualizer::addDirToGeometrySearchPaths(std::string(OPENSIM_ROOT_DIR) + "/Geometry");
		
		ModelBuilder builder("Model/gait2354_simbody.osim");
		builder.addIMU();
		builder.addMuscleMetabolicProbe();
		builder.addExoskeleton();
		builder.addActuators();
		builder.setModelName("BioGenius2026");
		builder.print("Results/biogenius2026.osim");
	}
	catch(const std::exception& e) 
	{
		std::cerr << std::format("[error] Aborting after catching an unhandled '{}':\n{}\n", typeid(e).name(), e.what());

        return 1;
	}
	
    return 0;
}
