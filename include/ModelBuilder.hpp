/**
 * @file ModelBuilder.hpp
 * @brief Declaration of the ModelBuilder class
 * @details The ModelBuilder takes a base model and has functions for adding
 *          the exoskeleton, a muscle metabolic probe, actuators and IMUs
 *
 * @author Samuel Savaria
 * @date 2026-08-25
*/
#ifndef BIOGENIUS_SIMULATION_MODEL_BUILDER_HPP
#define BIOGENIUS_SIMULATION_MODEL_BUILDER_HPP

#include <OpenSim/Simulation/Model/Model.h>
#include <OpenSim/Simulation/Model/Bhargava2004MuscleMetabolicsProbe.h>
#include <OpenSim/Simulation/SimbodyEngine/WeldJoint.h>
#include <OpenSim/Simulation/OpenSense/IMU.h>
#include <OpenSim/Actuators/CoordinateActuator.h>
#include <string>
#include <list>

/**
 * @brief ModelBuilder takes a base model and has functions for adding the
 *        exoskeleton, a muscle metabolic probe, actuators and IMUs
 */
class ModelBuilder
{
public:
	/**
	 * @brief Default constructs every member
	 */
	ModelBuilder() = default;

	/**
	 * @brief Initialize the model from a base model
	 * @param filename The path to the base model
	 */
	ModelBuilder(const std::string& filename);

	/**
	 * @brief Set the model to use as a base model and load it
	 * @param filename The path to the base model
	 */
	void setModel(const std::string& filename);

	/**
	 * @brief Change the name of the model being built
	 * @param name The new name of the model
	 */
	void setModelName(const std::string& name);

	/**
	 * @brief Save the current state of the model to a XML file
	 * @param filename The path to the file to create
	 */
	void print(const std::string& filename);

	/**
	 * @brief Show the model in the visualizer
	 */
	void visualize();

	/**
	 * @brief Add a muscle metabolic probe that tracks each muscle in the model
	 */
	void addMuscleMetabolicProbe();

	/**
	 * @brief Add the exoskeleton as a separate body
	 */
	void addExoskeleton();

	/**
	 * @brief Add actuators to the hip and knee joints to simulate motors
	 */
	void addActuators();

	/**
	 * @brief Add IMUs
	 */
	void addIMU();

private:
	OpenSim::Model m_model; //< The model to modify

	std::list<OpenSim::Bhargava2004MuscleMetabolicsProbe> m_probes;
	std::list<OpenSim::Body> m_bodies;
	std::list<OpenSim::WeldJoint> m_joints;
	std::list<OpenSim::Mesh> m_meshes;
	std::list<OpenSim::CoordinateActuator> m_actuators;
};

#endif
