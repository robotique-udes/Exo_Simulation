/**
 * @file ModelBuilder.hpp
 * @brief Declaration of the ModelBuilder class
 * @details The ModelBuilder takes a base model and a components file
 *          ModelBuilder adds the components from the components file to the base model
 *
 * @author Samuel Savaria
 * @date 2026-08-25
*/
#ifndef BIOGENIUS_SIMULATION_MODEL_BUILDER_HPP
#define BIOGENIUS_SIMULATION_MODEL_BUILDER_HPP

#include <OpenSim/Simulation/Model/Model.h>
#include <memory>
#include <string>

/**
 * @brief The ModelBuilder takes a base model and a components file.
 *        ModelBuilder adds the components from the components file to the base model.
 */
class ModelBuilder
{
public:
	/**
	 * @brief Default constructs every member
	 */
	ModelBuilder() = default;

	/**
	 * @brief Convenience constructor that sets the model and components file names
	 * @param modelFileName      The path to the base model
	 * @param componentsFileName The path to the components file
	 */
	ModelBuilder(const std::string& modelFileName, const std::string& componentsFileName) noexcept;

	/**
	 * @brief Set the model to use as a base model
	 * @param filename The path to the base model
	 */
	void setModelFileName(const std::string& filename) noexcept;

	/**
	 * @brief Set the components file
	 * @param filename The path to the components file
	 */
	void setComponentsFileName(const std::string& filename) noexcept;

	/*
	 * @brief Loads the model and add the components from the components file 
	 */
	void run();

	/**
	 * @brief Save the current state of the model to a XML file
	 * @param filename The path to the file to create
	 */
	void print(const std::string& filename);

	/**
	 * @brief Show the model in the visualizer
	 */
	void visualize();

private:
	/**
	 * @brief Helper function to parse a string containing 3 doubles separated by whitespaces as a Vec3
	 * @param raw The raw string containing 3 doubles separated by whitespaces
	 * @return The parsed Vec3
	 */
	static SimTK::Vec3 stringToVec3(const std::string& raw);

	/**
	 * @brief Helper function to parse a string containing 6 doubles separated by whitespaces as an Inertia
	 * @param raw The raw string containing 6 doubles separated by whitespaces
	 * @return The parsed Inertia
	 */
	static SimTK::Inertia stringToInertia(const std::string& raw);

	/**
	 * @brief Change the name of the model being built
	 * @param name The new name of the model
	 */
	void setModelName(const std::string& name);

	/**
	 * @brief Add a muscle metabolic probe that tracks each muscle in the base model
	 * @param name The name of the probe
	 */
	void addMuscleMetabolicProbe(const std::string& name);

	/**
	 * @brief Add a piece of the exoskeleton to the base model
	 * @param name        The name of the exeskeleton body
	 * @param parentBody  The name of the body to weld the exoskeleton to
	 * @param meshFile    The name of the STL file to use for the exoskeleton's geometry
	 * @param mass        The mass in kg
	 * @param massCenter  The center of mass relative to the origin of the piece, in meters
	 * @param inertia     The inertia relative to the center of mass, in kg * m^2
	 * @param position    The position of the exoskeleton relative to the parent body, in meters
	 * @param orientation The orientation of the exoskeleton relative to the parent body, in radians
	 */
	void addExoskeleton(const std::string& name,
		                const std::string& parentBody,
		                const std::string& meshFile,
		                double mass,
		                SimTK::Vec3 massCenter,
		                SimTK::Inertia inertia,
		                SimTK::Vec3 position,
		                SimTK::Vec3 orientation);

	/**
	 * @brief Add a motor (coordiante actuator) to the base model
	 * @param motorName  The name of the motor
	 * @param coordiante The coordinate to attach the motor to
	 */
	void addMotor(const std::string& motorName, const std::string& coordinate);

	/**
	 * @brief Add a IMU to the base model
	 * @param name        The name of the IMU
	 * @param parent      The name of the body to attach the IMU to
	 * @param position    The position of the IMU relative to the parent body
	 * @param orientation The orientation of the IMU relative to the parent body
	 */
	void addIMU(const std::string& name,
		        const std::string& parent,
		        SimTK::Vec3 position,
		        SimTK::Vec3 orientation);

	std::string m_modelFileName; //< The path to the base model file
	std::string m_componentsFileName; //< The path to the file of components
	std::unique_ptr<OpenSim::Model> m_model; ///< The model to modify
};

#endif
