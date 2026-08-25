/**
 * @file main.cpp
 * @brief Entry point of the simulation. Lists each step of the simulation
 *
 * @author Samuel Savaria
 * @date 2026-08-21
*/
#include <OpenSim/OpenSim.h>
#include <filesystem>
#include <iostream>
#include <chrono>
#include <format>

int main()
{
	try
	{
		std::filesystem::create_directory("Results");
		OpenSim::ModelVisualizer::addDirToGeometrySearchPaths("Geometry");
		OpenSim::ModelVisualizer::addDirToGeometrySearchPaths(std::string(OPENSIM_ROOT_DIR) + "/Geometry");
		

		std::cout << "Hello World!" << std::endl;
	}
	catch(const std::exception& e) 
	{
		// To-Do: Turn this into dedicated logging function
		std::string timestamp = std::format("{:%F %T}", std::chrono::system_clock::now());
		std::string type = typeid(e).name();
		std::string message = e.what();

        std::cerr << '[' << timestamp << "] [error] Aborting after catching an unhandled '" << type << "':" << std::endl;
		std::cerr << message << std::endl;

        return 1;
	}
	
    return 0;
}
