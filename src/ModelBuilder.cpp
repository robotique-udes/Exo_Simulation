/**
 * @file ModelBuilder.cpp
 * @brief Definition of the ModelBuilder class
 *
 * @author Samuel Savaria
 * @date 2026-08-25
*/
#include "ModelBuilder.hpp"
#include <OpenSim/Simulation/Model/Bhargava2004MuscleMetabolicsProbe.h>
#include <OpenSim/Actuators/CoordinateActuator.h>

ModelBuilder::ModelBuilder(const std::string& filename)
{
	m_model = OpenSim::Model(filename);
}

void ModelBuilder::print(const std::string& filename)
{
	m_model.finalizeConnections();
	m_model.print(filename);
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

}

void ModelBuilder::addActuators()
{
	// Ownerless object. Will be freed when program ends, same as in exampleHangingMuscle.cpp
	OpenSim::CoordinateActuator* hipRight = new OpenSim::CoordinateActuator("hip_flexion_r");
	hipRight->setName("exo_hip_r");
	hipRight->setOptimalForce(1.0);
	hipRight->setMinControl(-15.0);
	hipRight->setMaxControl(15.0);

	// Ownerless object. Will be freed when program ends, same as in exampleHangingMuscle.cpp
	OpenSim::CoordinateActuator* hipLeft = new OpenSim::CoordinateActuator("hip_flexion_l");
	hipLeft->setName("exo_hip_l");
	hipLeft->setOptimalForce(1.0);
	hipLeft->setMinControl(-15.0);
	hipLeft->setMaxControl(15.0);

	// Ownerless object. Will be freed when program ends, same as in exampleHangingMuscle.cpp
	OpenSim::CoordinateActuator* kneeRight = new OpenSim::CoordinateActuator("knee_angle_r");
	kneeRight->setName("exo_knee_r");
	kneeRight->setOptimalForce(1.0);
	kneeRight->setMinControl(-15.0);
	kneeRight->setMaxControl(15.0);

	// Ownerless object. Will be freed when program ends, same as in exampleHangingMuscle.cpp
	OpenSim::CoordinateActuator* kneeLeft = new OpenSim::CoordinateActuator("knee_angle_l");
	kneeLeft->setName("exo_knee_l");
	kneeLeft->setOptimalForce(1.0);
	kneeLeft->setMinControl(-15.0);
	kneeLeft->setMaxControl(15.0);

	m_model.addForce(hipRight);
	m_model.addForce(hipLeft);
	m_model.addForce(kneeRight);
	m_model.addForce(kneeLeft);
}

void ModelBuilder::addIMU()
{

}
