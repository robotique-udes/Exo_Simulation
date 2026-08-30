/**
 * @file ModelBuilder.cpp
 * @brief Definition of the ModelBuilder class
 *
 * @author Samuel Savaria
 * @date 2026-08-25
*/
#include "ModelBuilder.hpp"
#include <numbers>

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

	OpenSim::Bhargava2004MuscleMetabolicsProbe& probe = m_probes.emplace_back();
	probe.set_report_total_metabolics_only(false);
	probe.setName("metabolics");

	const OpenSim::Set<OpenSim::Muscle>& muscles = m_model.getMuscles();
	for(int i = 0; i < muscles.getSize(); ++ i)
	{
		const std::string& muscle = muscles[i].getName();
		probe.addMuscle(muscle, 0.5, slow_activation, fast_activation, slow_maintenance, fast_maintenance);
	}

	m_model.addProbe(&probe);
}

void ModelBuilder::addExoskeleton()
{
	/**
	 * @brief Local POD struct for creating the pieces of the exoskeleton
	 */
	struct ExoskeletonPiece
	{
		std::string name;
		std::string parentBody;
		std::string meshFile;

		double mass; // kilograms
		SimTK::Vec3 massCenter; // meters
		SimTK::Inertia inertia; // kilograms * meters squared

		SimTK::Vec3 position; // meters
		SimTK::Vec3 orientation; // radians
	};

	using namespace std::numbers; // For easier access to pi
	const ExoskeletonPiece PIECES[] = { 
		                     /* Hips */ {.name = "exo_hip", .parentBody = "pelvis", .meshFile = "pelvis.stl", .mass = 3.915, .massCenter = {-0.00025, -0.0553, -0.000616}, .inertia = {0.0232, 0.0546, 0.0669, 0.0000792, 0.0015, 0.00191}, .position = {-0.3, -0.015, 0}, .orientation = {pi / 2, -pi / 2, 0.0}},
					  /* Right femur */ {.name = "exo_femur_r", .parentBody = "femur_r", .meshFile = "femur.stl", .mass = 2.507, .massCenter = {-0.000102, 0.0987, 0.0119}, .inertia = {0.0707, 0.00232, 0.0711, 0.000107, -0.00000677, 0.00275}, .position = {0, 0.05, 0.1}, .orientation = {0, 0, pi}},
					  /* Right tibia */ {.name = "exo_tibia_r", .parentBody = "tibia_r", .meshFile = "tibia.stl", .mass = 2.304, .massCenter = {0.0265, -0.000025, 0.0112}, .inertia = {0.00231, 0.0136, 0.014, -0.00000564, 0.000977, 0.00000529}, .position = {0.005, -0.045, 0.095}, .orientation = {0, 0, pi/2}},
					   /* Left femur */ {.name = "exo_femur_l", .parentBody = "femur_l", .meshFile = "femur.stl", .mass = 2.507, .massCenter = {-0.000102, 0.0987, 0.0119}, .inertia = {0.0707, 0.00232, 0.0711, 0.000107, -0.00000677, 0.00275}, .position = {0, 0.05, -0.1}, .orientation = {0, pi, pi}},
					   /* Left tibia */ {.name = "exo_tibia_l", .parentBody = "tibia_l", .meshFile = "tibia.stl", .mass = 2.304, .massCenter = {0.0265, -0.000025, 0.0112}, .inertia = {0.00231, 0.0136, 0.014, -0.00000564, 0.000977, 0.00000529}, .position = {0.005, -0.045, -0.095}, .orientation = {0, pi, -pi/2}},
	                                  };

	for (const ExoskeletonPiece& piece : PIECES)
	{
		OpenSim::Mesh& mesh = m_meshes.emplace_back(piece.meshFile);
		OpenSim::Body& body = m_bodies.emplace_back(piece.name, 
			                                       piece.mass, 
			                                       piece.massCenter,
			                                       piece.inertia);
		OpenSim::WeldJoint& joint = m_joints.emplace_back(piece.name, 
			                                              m_model.getBodySet().get(piece.parentBody),
			                                              piece.position, 
			                                              SimTK::Vec3(0, 0, 0), 
			                                              body, 
			                                              SimTK::Vec3(0, 0, 0),
			                                              piece.orientation);

		m_model.addBody(&body);
		m_model.addJoint(&joint);
		body.attachGeometry(&mesh); // Called after Model::addBody to prevent "[error] Mesh xxx.stl not connected to a model...ignoring"
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

	constexpr CoordinateActuatorPair PAIRS[] = { {.coordinateName = "hip_flexion_r", .actuatorName = "exo_hip_r"},
											     {.coordinateName = "hip_flexion_l", .actuatorName = "exo_hip_l"},
											     {.coordinateName = "knee_angle_r", .actuatorName = "exo_knee_r"},
											     {.coordinateName = "knee_angle_l", .actuatorName = "exo_knee_l"}};

	for (const CoordinateActuatorPair& pair : PAIRS)
	{
		OpenSim::CoordinateActuator& actuator = m_actuators.emplace_back(pair.coordinateName);
		actuator.setName(pair.actuatorName);
		actuator.setOptimalForce(1.0);
		actuator.setMinControl(-15.0);
		actuator.setMaxControl(15.0);

		m_model.addForce(&actuator);
	}
}

void ModelBuilder::addIMU()
{

}
