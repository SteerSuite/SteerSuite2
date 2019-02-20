//
// Copyright (c) 2009-2018 Brandon Haworth, Glen Berseth, Muhammad Usman, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//



#ifndef __SocialForces_AI_MODULE__
#define __SocialForces_AI_MODULE__

/// @file SimpleAIModule.h
/// @brief Declares the SimpleAIModule plugin.

#include "planning/SteerSuiteGridAStar.h"
#include "SteerLib.h"
#include <vector>
#include "SocialForces_Parameters.h"
#include "Logger.h"


// globally accessible to the simpleAI plugin
// extern SteerLib::EngineInterface * gEngine;
// extern SteerLib::SpatialDataBaseInterface * gSpatialDatabase;



/**
 * @brief An example plugin for the SimulationEngine that provides very basic AI agents.
 *
 * This class is an example of a plug-in module (as opposed to a built-in module).
 * It compiles as part of a dynamic library which is loaded by a SimulationEngine at run-time.
 *
 * The simpleAI plugin consists of three parts:
 *  - This class inherits from SteerLib::ModuleInterface, and implements only the desired functionality.  In this case
 *    the desired functionality is to be able to create/destroy SimpleAgent agents.
 *  - The two global functions createModule() and destroyModule() are implemented so that the engine can load the
 *    dynamic library and get an instance of our SimpleAIModule.
 *  - The SimpleAgent class inherits from SteerLib::AgentInterface, which is the agent steering AI used by the engine.
 *    this agent serves as an example of how to create your own steering AI using SteerLib features.
 *
 */
 namespace SocialForcesGlobals {
	extern SteerSuiteGridEnvironment * gEnvironmentForAStar;

	extern unsigned int gLongTermPlanningPhaseInterval;
	extern unsigned int gMidTermPlanningPhaseInterval;
	extern unsigned int gShortTermPlanningPhaseInterval;
	extern unsigned int gPredictivePhaseInterval;
	extern unsigned int gReactivePhaseInterval;
	extern unsigned int gPerceptivePhaseInterval;
	extern bool gUseDynamicPhaseScheduling;
	extern bool gShowStats;
	extern bool gShowAllStats;
	extern bool dont_plan;


	// Adding a bunch of parameters so they can be changed via input
	extern float sf_acceleration;
	extern float sf_personal_space_threshold;
	extern float sf_agent_repulsion_importance;
	extern float sf_query_radius;
	extern float sf_body_force;
	extern float sf_agent_body_force;
	extern float sf_sliding_friction_force;
	extern float sf_agent_b;
	extern float sf_agent_a;
	extern float sf_wall_b;
	extern float sf_wall_a;
	extern float sf_max_speed;


	extern PhaseProfilers * gPhaseProfilers;
}


class SocialForcesAIModule : public SteerLib::ModuleInterface
{
public:
	
	void cleanupSimulation();
	void preprocessSimulation();
	void initializeSimulation();
	//std::string getDependencies() { return "testCasePlayer"; }
	std::string getDependencies() { return ""; }
	
	std::string getConflicts() { return ""; }
	std::string getData() { return _data; }
	LogData * getLogData()
	{
		LogData * lD = new LogData();
		lD->setLogger(this->_rvoLogger);
		lD->setLogData(this->_logData);
		return lD;
	}
	void init( const SteerLib::OptionDictionary & options, SteerLib::EngineInterface * engineInfo );
	void finish();
	SteerLib::AgentInterface * createAgent();
	void destroyAgent( SteerLib::AgentInterface * agent );

	void preprocessFrame(float timeStamp, float dt, unsigned int frameNumber);
	void postprocessFrame(float timeStamp, float dt, unsigned int frameNumber);
	std::vector<SteerLib::AgentInterface * > agents_;

	Util::PerformanceProfiler *  getAIProfiler();

protected:
	std::string logFilename; // = "pprAI.log";
	bool logStats; // = false;
	Logger * _rvoLogger;
	std::string _data;
	std::vector<LogObject *> _logData;

	SteerLib::EngineInterface * _gEngine;
};

#endif
