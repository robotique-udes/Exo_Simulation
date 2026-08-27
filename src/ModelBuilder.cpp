/**
 * @file ModelBuilder.cpp
 * @brief Definition of the ModelBuilder class
 *
 * @author Samuel Savaria
 * @date 2026-08-25
*/
#include "ModelBuilder.hpp"
#include <numbers>
#include <OpenSim/Simulation/Model/Bhargava2004MuscleMetabolicsProbe.h>
#include <OpenSim/Actuators/CoordinateActuator.h>

ModelBuilder::ModelBuilder(const std::string& p_filename)
{
	m_model = OpenSim::Model(p_filename);
}

void ModelBuilder::print(const std::string& p_filename)
{
	m_model.finalizeConnections();
	m_model.print(p_filename);
}

void ModelBuilder::visualize()
{
	m_model.setUseVisualizer(true);

	SimTK::State& state = m_model.initSystem();
	m_model.getVisualizer().show(state);
}

void ModelBuilder::addMuscleMetabolicProbe()
{
	auto defaultParameters = OpenSim::Bhargava2004MuscleMetabolicsProbe_MetabolicMuscleParameter();
	const double slow_activation = defaultParameters.get_activation_constant_slow_twitch();
	const double fast_activation = defaultParameters.get_activation_constant_fast_twitch();
	const double slow_maintenance = defaultParameters.get_maintenance_constant_slow_twitch();
	const double fast_maintenance = defaultParameters.get_maintenance_constant_fast_twitch();

	// Ownerless object. Will be freed when program ends, same as in exampleHangingMuscle.cpp
	auto probe = new OpenSim::Bhargava2004MuscleMetabolicsProbe();
	probe->set_report_total_metabolics_only(false);
	probe->setName("metabolics");

	const OpenSim::Set<OpenSim::Muscle>& muscles = m_model.getMuscles();
	for(int i = 0; i < muscles.getSize(); ++ i)
	{
		const std::string& muscle = muscles[i].getName();
		probe->addMuscle(muscle, 0.5, slow_activation, fast_activation, slow_maintenance, fast_maintenance);
	}

	m_model.addProbe(probe);
}

void ModelBuilder::addExoskeleton()
{
	/**
	 * @brief Local POD struct for creating the pieces of the exoskeleton
	 */
	struct ExoskeletonPiece
	{
		std::string meshFile;
		std::string name;
		std::string parentBody;

		double mass; // kilograms
		SimTK::Vec3 massCenter; // meters
		SimTK::Inertia inertia; // kilograms * meters squared

		SimTK::Vec3 position; // meters
		SimTK::Vec3 orientation; // radians
	};

	using namespace std::numbers; // For easier access to pi
	const ExoskeletonPiece PIECES[] = { {"file.stl", "name", "bodypart", 0.0, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}} };

	for (const ExoskeletonPiece& piece : PIECES)
	{
		OpenSim::Mesh* mesh = new OpenSim::Mesh(piece.meshFile);
		OpenSim::Body* body = new OpenSim::Body(piece.name + "_body", 
			                                    piece.mass, 
			                                    piece.massCenter,
			                                    piece.inertia);
		OpenSim::Joint* joint = new OpenSim::WeldJoint(piece.name + "_joint", 
			                                           m_model.getBodySet().get(piece.parentBody),
			                                           { 0, 0, 0 }, 
			                                           { 0, 0, 0 }, 
			                                           *body, 
			                                           piece.position, 
			                                           piece.orientation);

		m_model.addBody(body);
		m_model.addJoint(joint);
		body->attachGeometry(mesh); // Called after Model::addBody to prevent "[error] Mesh xxx.stl not connected to a model...ignoring"
	}
}

void ModelBuilder::addActuators()
{
	/**
	 * @brief Local POD struct for attaching a CoordinateActuator to a Coordinate
	 */
	struct CoordinateActuatorPair
	{
		std::string coordinateName;
		std::string actuatorName;
	};

	constexpr CoordinateActuatorPair PAIRS[] = { {"hip_flexion_r", "exo_hip_r"},
											     {"hip_flexion_l", "exo_hip_l"},
											     {"knee_angle_r", "exo_knee_r"}, 
											     {"knee_angle_l", "exo_knee_l"}};

	for (const CoordinateActuatorPair& pair : PAIRS)
	{
		// Ownerless object. Will be freed when program ends, same as in exampleHangingMuscle.cpp
		OpenSim::CoordinateActuator* actuator = new OpenSim::CoordinateActuator(pair.coordinateName);
		actuator->setName(pair.actuatorName);
		actuator->setOptimalForce(1.0);
		actuator->setMinControl(-15.0);
		actuator->setMaxControl(15.0);

		m_model.addForce(actuator);
	}
}

void ModelBuilder::addIMU()
{

}
