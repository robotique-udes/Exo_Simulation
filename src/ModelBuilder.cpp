/**
 * @file ModelBuilder.cpp
 * @brief Definition of the ModelBuilder class
 *
 * @author Samuel Savaria
 * @date 2026-08-25
*/
#include "ModelBuilder.hpp"
#include <OpenSim/Simulation/Model/Bhargava2004MuscleMetabolicsProbe.h>
#include <OpenSim/Simulation/SimbodyEngine/WeldJoint.h>
#include <OpenSim/Simulation/OpenSense/IMU.h>
#include <OpenSim/Actuators/CoordinateActuator.h>
#include <pugixml.hpp>

#include <numbers>

SimTK::Vec3 ModelBuilder::stringToVec3(const std::string& raw)
{
	double components[3];
	std::size_t currentIndex = 0;
	std::size_t nextWhiteSpace = 0;

	for (std::size_t i = 0; i < 3; ++i)
	{
		currentIndex = nextWhiteSpace;
		nextWhiteSpace = raw.find_first_of(' ', currentIndex + 1);
		components[i] = std::stod(raw.substr(currentIndex, nextWhiteSpace - currentIndex));
	}

	return SimTK::Vec3(components[0], components[1], components[2]);
}

SimTK::Inertia ModelBuilder::stringToInertia(const std::string& raw)
{
	double components[6];
	std::size_t currentIndex = 0;
	std::size_t nextWhiteSpace = 0;

	for (std::size_t i = 0; i < 6; ++i)
	{
		currentIndex = nextWhiteSpace;
		nextWhiteSpace = raw.find_first_of(' ', currentIndex + 1);
		components[i] = std::stod(raw.substr(currentIndex, nextWhiteSpace - currentIndex));
	}

	return SimTK::Inertia(components[0], components[1], components[2], components[3], components[4], components[5]);
}

ModelBuilder::ModelBuilder(const std::string& p_modelFileName, const std::string& p_componentsFileName) noexcept
{
	setModelFileName(p_modelFileName);
	setComponentsFileName(p_componentsFileName);
}

void ModelBuilder::setModelFileName(const std::string& p_filename) noexcept
{
	m_modelFileName = p_filename;
}

void ModelBuilder::setComponentsFileName(const std::string& p_filename) noexcept
{
	m_componentsFileName = p_filename;
}

void ModelBuilder::run()
{
	// Load the base model
	m_model = std::make_unique<OpenSim::Model>(m_modelFileName);
	m_model->finalizeFromProperties(); // Make sure the model has full ownership of its subcomponents

	// Load the components file
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(m_componentsFileName.c_str());
	if (!result)
	{
		throw std::runtime_error(std::format("{}:{}\n{}", m_componentsFileName, result.offset, result.description()));
	}

	pugi::xml_node builder = doc.child("SimulationDocument").child("ModelBuilder");

	// Set model name
	const char* modelName = builder.attribute("name").value();
	setModelName(modelName);

	// Add the exoskeleton
	for (pugi::xml_node& exoskeleton : builder.child("ExoskeletonSet").children())
	{
		std::string name = exoskeleton.child_value("name");
		std::string parentBody = exoskeleton.child_value("parent");
		std::string meshFile = exoskeleton.child_value("mesh");
		double mass = std::stod(exoskeleton.child_value("mass"));
		SimTK::Vec3 massCenter = stringToVec3(exoskeleton.child_value("mass_center"));
		SimTK::Inertia inertia = stringToInertia(exoskeleton.child_value("inertia"));
		SimTK::Vec3 position = stringToVec3(exoskeleton.child_value("position"));
		SimTK::Vec3 orientation = stringToVec3(exoskeleton.child_value("orientation"));

		addExoskeleton(name,
			           parentBody,
			           meshFile,
			           mass,
			           massCenter,
			           inertia,
			           position,
			           orientation);
	}

	// Add the motors
	for (pugi::xml_node& motor : builder.child("MotorSet").children())
	{
		std::string name = motor.child_value("name");
		std::string coordinate = motor.child_value("coordinate");

		addMotor(name, coordinate);
	}

	// Add the IMUs
	for (pugi::xml_node& imu : builder.child("IMUSet").children())
	{
		std::string name = imu.child_value("name");
		std::string parent = imu.child_value("parent");
		SimTK::Vec3 position = stringToVec3(imu.child_value("position"));
		SimTK::Vec3 orientation = stringToVec3(imu.child_value("orientation"));

		addIMU(name, parent, position, orientation);
	}

	// Add the probes
	for (pugi::xml_node& imu : builder.child("ProbeSet").children())
	{
		std::string name = imu.child_value("name");

		addMuscleMetabolicProbe(name);
	}
}

void ModelBuilder::print(const std::string& p_filename)
{
	m_model->finalizeConnections();
	m_model->print(p_filename);
}

void ModelBuilder::visualize()
{
	m_model->setUseVisualizer(true);

	SimTK::State& state = m_model->initSystem();
	m_model->getVisualizer().show(state);
}

void ModelBuilder::setModelName(const std::string& p_name)
{
	m_model->setName(p_name);
}

void ModelBuilder::addMuscleMetabolicProbe(const std::string& p_name)
{
	const auto defaultParameters = OpenSim::Bhargava2004MuscleMetabolicsProbe_MetabolicMuscleParameter();
	const double slow_activation = defaultParameters.get_activation_constant_slow_twitch();
	const double fast_activation = defaultParameters.get_activation_constant_fast_twitch();
	const double slow_maintenance = defaultParameters.get_maintenance_constant_slow_twitch();
	const double fast_maintenance = defaultParameters.get_maintenance_constant_fast_twitch();

	auto probe = std::make_unique<OpenSim::Bhargava2004MuscleMetabolicsProbe>();
	probe->set_report_total_metabolics_only(false);
	probe->setName(p_name);

	const OpenSim::Set<OpenSim::Muscle>& muscles = m_model->getMuscles();
	for(int i = 0; i < muscles.getSize(); ++ i)
	{
		const std::string& muscle = muscles[i].getName();
		probe->addMuscle(muscle, 0.5, slow_activation, fast_activation, slow_maintenance, fast_maintenance);
	}

	m_model->addProbe(probe.release());
}

void ModelBuilder::addExoskeleton(const std::string& p_name, 
	                              const std::string& p_parentBody,
	                              const std::string& p_meshFile,
	                              double p_mass,
	                              SimTK::Vec3 p_massCenter,
	                              SimTK::Inertia p_inertia,
	                              SimTK::Vec3 p_position,
	                              SimTK::Vec3 p_orientation)
{
	auto mesh = std::make_unique<OpenSim::Mesh>(p_meshFile);
	auto body = std::make_unique<OpenSim::Body>(p_name, 
												p_mass, 
												p_massCenter,
												p_inertia);
	auto joint = std::make_unique<OpenSim::WeldJoint>(p_name, 
														m_model->getBodySet().get(p_parentBody),
														p_position, 
														SimTK::Vec3(0, 0, 0), 
														*body, 
														SimTK::Vec3(0, 0, 0),
														p_orientation);

	body->attachGeometry(mesh.release()); // "[error] Mesh xxx.stl not connected to a model...ignoring" can be ignored and has no effect
	m_model->addBody(body.release());
	m_model->addJoint(joint.release());
}

void ModelBuilder::addMotor(const std::string& p_motorName, const std::string& p_coordinateName)
{
	auto actuator = std::make_unique<OpenSim::CoordinateActuator>(p_coordinateName);
	actuator->setName(p_motorName);
	actuator->setOptimalForce(1.0);
	actuator->setMinControl(-15.0);
	actuator->setMaxControl(15.0);

	m_model->addForce(actuator.release());
}

void ModelBuilder::addIMU(const std::string& p_name,
	                      const std::string& p_parent,
	                      SimTK::Vec3 p_position,
	                      SimTK::Vec3 p_orientation)
{
	// Add the IMU to the model
	SimTK::Rotation rotation;
	rotation.setRotationToBodyFixedXYZ(p_orientation);
	OpenSim::Body& parent = m_model->updBodySet().get(p_parent);
	auto imu = std::make_unique<OpenSim::IMU>();
	auto frame = std::make_unique<OpenSim::PhysicalOffsetFrame>(p_name + "_frame",
			                                                    parent,
			                                                    SimTK::Transform(rotation, p_position));
	imu->setName(p_name);
	imu->connectSocket_frame(*frame);
	parent.addComponent(frame.release());
	m_model->addModelComponent(imu.release());

	// Print the orientation of the IMU as a quaternion
	m_model->finalizeConnections();
	SimTK::State& state = m_model->initSystem();
	m_model->realizeAcceleration(state);

	const OpenSim::IMU& ref = m_model->getComponent<OpenSim::IMU>("/componentset/" + p_name);
	SimTK::Quaternion orientation = ref.getOutputValue<SimTK::Quaternion>(state, "orientation_as_quaternion");
	std::cout << std::format("[info] {}: {}\n", p_name, orientation.toString());
}
