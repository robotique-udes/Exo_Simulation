/**
 * @file ModelBuilder.cpp
 * @brief Definition of the ModelBuilder class
 *
 * @author Samuel Savaria
 * @date 2026-08-25
*/
#include "ModelBuilder.hpp"
#include <numbers>

ModelBuilder::ModelBuilder(const std::string& p_filename) : m_model(p_filename)
{

}

void ModelBuilder::setModel(const std::string& p_filename)
{
	m_model = OpenSim::Model(p_filename);
	m_model.finalizeFromProperties(); // Make sure the model has full ownership of its subcomponents
}

void ModelBuilder::setModelName(const std::string& p_name)
{
	m_model.setName(p_name);
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
		std::string name;        //< The name of the exeskeleton body
		std::string parentBody;  //< The name of the body to weld the exoskeleton to
		std::string meshFile;    //< The name of the STL file to use for the exoskeleton's geometry

		double mass;             //< The mass in kg
		SimTK::Vec3 massCenter;  //< The center of mass relative to the origin of the piece, in meters
		SimTK::Inertia inertia;  //< The inertia relative to the center of mass, in kg * m^2

		SimTK::Vec3 position;    //< The position of the exoskeleton relative to the parent body, in meters
		SimTK::Vec3 orientation; // The orientation of the exoskeleton relative to the parent body, in radians
	};

	using namespace std::numbers; // For easier access to pi
	const ExoskeletonPiece PIECES[] = { 
		                     /* Hips */ {.name = "exo_hip", .parentBody = "pelvis", .meshFile = "pelvis.stl", .mass = 3.915, .massCenter = {-0.00025, -0.0553, -0.000616}, .inertia = {0.0232, 0.0546, 0.0669, 0.0000792, 0.0015, 0.00191}, .position = {-0.3, -0.015, 0}, .orientation = {pi / 2, -pi / 2, 0.0}},
		              /* Right thigh */ {.name = "exo_thigh_r", .parentBody = "femur_r", .meshFile = "femur.stl", .mass = 2.507, .massCenter = {-0.000102, 0.0987, 0.0119}, .inertia = {0.0707, 0.00232, 0.0711, 0.000107, -0.00000677, 0.00275}, .position = {0, 0.05, 0.1}, .orientation = {0, 0, pi}},
		              /* Right shank */ {.name = "exo_shank_r", .parentBody = "tibia_r", .meshFile = "tibia.stl", .mass = 2.304, .massCenter = {0.0265, -0.000025, 0.0112}, .inertia = {0.00231, 0.0136, 0.014, -0.00000564, 0.000977, 0.00000529}, .position = {0.005, -0.045, 0.095}, .orientation = {0, 0, pi / 2}},
		               /* Left thigh */ {.name = "exo_thigh_l", .parentBody = "femur_l", .meshFile = "femur.stl", .mass = 2.507, .massCenter = {-0.000102, 0.0987, 0.0119}, .inertia = {0.0707, 0.00232, 0.0711, 0.000107, -0.00000677, 0.00275}, .position = {0, 0.05, -0.1}, .orientation = {0, pi, pi}},
		               /* Left shank */ {.name = "exo_shank_l", .parentBody = "tibia_l", .meshFile = "tibia.stl", .mass = 2.304, .massCenter = {0.0265, -0.000025, 0.0112}, .inertia = {0.00231, 0.0136, 0.014, -0.00000564, 0.000977, 0.00000529}, .position = {0.005, -0.045, -0.095}, .orientation = {0, pi, -pi / 2}},
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
		std::string actuatorName;   //< The name of the actuator/motor
		std::string coordinateName; //< The name of the coordinate the actuator will be attached to
	};

	const CoordinateActuatorPair PAIRS[] = { {.actuatorName = "exo_hip_motor_r", .coordinateName = "hip_flexion_r"},
											 {.actuatorName = "exo_hip_motor_l", .coordinateName = "hip_flexion_l"},
											 {.actuatorName = "exo_knee_motor_r", .coordinateName = "knee_angle_r"},
											 {.actuatorName = "exo_knee_motor_l", .coordinateName = "knee_angle_l"} };

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
	/**
	 * @brief Local POD struct for attaching an IMU to a Body
	 */
	struct BIMU
	{
		std::string name;       //< The name of the BIMU
		std::string parentBody; //< The name of the body to attach the BIMU to
		SimTK::Vec3 position;   //< The position of the BIMU relative to the parent body
	};

	const BIMU bimus[] = { {.name = "imu_back", .parentBody = "torso", .position = {-0.125, 0.1, 0.15}},
		                   {.name = "imu_thigh_r", .parentBody = "femur_r", .position = {0, -0.35, 0.11}},
		                   {.name = "imu_thigh_l", .parentBody = "femur_l", .position = {0, -0.35, -0.11}},
						   {.name = "imu_shank_r", .parentBody = "tibia_r", .position = {0.005, -0.4, 0.1}},
						   {.name = "imu_shank_l", .parentBody = "tibia_l", .position = {0.005, -0.4, -0.1}} };

	for (const BIMU& bimu : bimus)
	{
		OpenSim::IMU& imu = m_sensors.emplace_back();
		OpenSim::Body& parent = m_model.updBodySet().get(bimu.parentBody);
		OpenSim::PhysicalOffsetFrame& frame = m_offsets.emplace_back(bimu.name + "_frame",
			                                                         parent,
			                                                         SimTK::Transform(bimu.position));

		imu.setName(bimu.name);
		imu.connectSocket_frame(frame);
		parent.addComponent(&frame);
		m_model.addComponent(&imu);
	}

	// Add IMU Placer
}
