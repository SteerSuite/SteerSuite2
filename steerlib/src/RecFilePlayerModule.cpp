//
// Copyright (c) 2009-2018 Brandon Haworth, Glen Berseth, Muhammad Usman, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//



/// @file RecFilePlayerModule.cpp
/// @brief Implements the RecFilePlayerModule built-in module.

#include "SteerLib.h"
#include "simulation/SimulationOptions.h"
#include "modules/RecFilePlayerModule.h"
#include <iostream>

using namespace std;
using namespace SteerLib;
using namespace Util;

#define KEY_PRESSED 1
// #define _DEBUG 1

void RecFilePlayerModule::init( const SteerLib::OptionDictionary & options, SteerLib::EngineInterface * engineInfo )
{
	_recFilename = "";
	_playbackSpeed = 1.0;
	_engine = engineInfo;
	_dataFileName = "";
	_saveTestcaseName = false;
	_saveFirstAgentCompletionFrame = false;
	_firstAgentCompletionFrame = -1;

	// parse the options
	SteerLib::OptionDictionary::const_iterator optionIter;
	for (optionIter = options.begin(); optionIter != options.end(); ++optionIter) {
		if ((*optionIter).first == "retime") {
			std::istringstream((*optionIter).second) >> _playbackSpeed;
			if (_playbackSpeed < 0.0) {
				throw GenericException("Playback speed cannot be negative. Requested playback speed was " + toString(_playbackSpeed));
			}
		}
		else if ((*optionIter).first == "recfile") {
			_recFilename = (*optionIter).second;
		}
		else if ((*optionIter).first == "setDataFile") {
			_dataFileName = (*optionIter).second;
		}
		else if ((*optionIter).first == "saveTestcaseName") {
			_saveTestcaseName = true;
		}
		else if ((*optionIter).first == "saveFirstAgentCompletionFrame") {
			_saveFirstAgentCompletionFrame = true;
		}
		else {
			throw Util::GenericException("unrecognized option \"" + Util::toString((*optionIter).first) + "\" given to recFilePlayer module.");
		}
	}

#ifdef ENABLE_GUI
#ifdef ENABLE_QT

	QMainWindow * mainWindow = (QMainWindow*)(_engine->getEngineController()->getQtMainWindow());

	if(mainWindow != NULL) {

		_recFilePlayerDockWidget = new QDockWidget("Rec File Player");
		_recFilePlayerWidget = new SteerSimQt::RecFilePlayerWidget(this, _engine);
		_recFilePlayerDockWidget->setWidget(_recFilePlayerWidget);
		mainWindow->addDockWidget(Qt::RightDockWidgetArea, _recFilePlayerDockWidget);
	}
	else {
		_recFilePlayerDockWidget = NULL;
		_recFilePlayerWidget = NULL;
	}
#endif
#endif
}

void RecFilePlayerModule::finish()
{
}


void RecFilePlayerModule::processKeyboardInput(int key, int action )
{
	if ((key == _engine->getOptions().keyboardBindings.stepBackward)  && (action==KEY_PRESSED)) {
		_currentTimeToPlayback -= 2.0f * _fixedTimeStep * _playbackSpeed;  // subtract twice because it will inevitably be added again when updating.
		if (_currentTimeToPlayback < _simulationStartTime) _currentTimeToPlayback = _simulationStartTime;
		if (_currentTimeToPlayback > _simulationStopTime) _currentTimeToPlayback = _simulationStopTime;
		_engine->getEngineController()->pauseAndStepOneFrame();
	}
	else if ((key == _engine->getOptions().keyboardBindings.slowdownPlayback) && (action==KEY_PRESSED)) {
		_playbackSpeed = _playbackSpeed * .8;
	}
	else if ((key == _engine->getOptions().keyboardBindings.speedupPlayback) && (action==KEY_PRESSED)) {
		_playbackSpeed = _playbackSpeed * 1.25;
	}
	else if ((key == _engine->getOptions().keyboardBindings.resetPlaybackSpeed) && (action==KEY_PRESSED)) {
		_playbackSpeed = 1.0f;
	}
	else if ((key == _engine->getOptions().keyboardBindings.restartPlayback) && (action==KEY_PRESSED)) {
		_currentTimeToPlayback = _simulationStartTime;
	}
	else {
		// do nothing
	}
}


void RecFilePlayerModule::initializeSimulation()
{
	// make sure a recfile was specified
	if (_recFilename == "") {
		throw Util::GenericException("No rec file specified for playback.");
	}
	_simulationReader = new SteerLib::RecFileReader( _recFilename );
	if (_simulationReader->getNumFrames() == 0) {
		throw GenericException("ReplayAIModule::init() - playback file " + _simulationReader->getFilename() + " has no frames, cannot replay.");
	}
	if (_simulationReader->getNumFrames() == 1) {
		throw GenericException("ReplayAIModule::init() - playback file " + _simulationReader->getFilename() + " has only one frame, cannot replay.");
	}

	// set some member vars
	_simulationStartTime = _simulationReader->getTimeStampForFrame(0);
	_simulationStopTime = _simulationReader->getTimeStampForFrame(_simulationReader->getNumFrames()-1);
	_currentTimeToPlayback = _simulationReader->getTimeStampForFrame(0);

	// for a fixed frame rate, the following default for is effectively 1 frame per step.
	_fixedTimeStep = _simulationReader->getTotalElapsedTime() / ((double)(_simulationReader->getNumFrames()-1));

	for (unsigned int i=0; i < _simulationReader->getNumObstacles(); i++) {
		BoxObstacle * b;
		b = new BoxObstacle(_simulationReader->getObstacleBoundsAtFrame(i,0));
		_obstacles.push_back(b);
		_engine->addObstacle(b);
		_engine->getSpatialDatabase()->addObject( b, b->getBounds());
	}

	//
	// allocate agents, initialize them to frame 0.
	//
	for (unsigned int i=0;  i < _simulationReader->getNumAgents(); i++) {
		ReplayAgent * agent = new ReplayAgent();

		/// @todo
		///   The next version of the RecFileReader should also return an AgentGoalInfo struct, and this indirection should be unnecessary.
		AgentGoalInfo newGoal;
		newGoal.targetLocation = _simulationReader->getAgentGoalAtFrame(i,0);

		agent->setPosition(_simulationReader->getAgentLocationAtFrame(i,0));
		agent->setForward(_simulationReader->getAgentOrientationAtFrame(i,0));
		agent->setEnabled(_simulationReader->isAgentEnabledAtFrame(i,0));
		agent->setRadius(_simulationReader->getAgentRadiusAtFrame(i,0));
		agent->setCurrentGoal(newGoal);
		// Best to start out with zero velocity until this support is move into recFileWriter format
		agent->setVelocity(Util::Vector(0,0,0));

		_engine->addAgent(agent,this);
		// And add to griddatabase
		Util::AxisAlignedBox newBounds(agent->position().x-agent->radius(),
				agent->position().x+agent->radius(), 0.0f, 0.5f,
				agent->position().z-agent->radius(), agent->position().z+agent->radius());
		_engine->getSpatialDatabase()->addObject( dynamic_cast<SpatialDatabaseItemPtr>(agent), newBounds);
	}

}

void RecFilePlayerModule::cleanupSimulation()
{
#ifdef _DEBUG
	std::cout << "about to RecFilePlayerModule::cleanupSimulation();\n";
#endif
	for (unsigned int i=0; i<_obstacles.size(); i++) {
		_engine->getSpatialDatabase()->removeObject( _obstacles[i], _obstacles[i]->getBounds());
		_engine->removeObstacle(_obstacles[i]);
		delete _obstacles[i];
	}
	_obstacles.clear();

	_engine->destroyAllAgentsFromModule(this);
}



void RecFilePlayerModule::preprocessFrame(float timeStamp, float dt, unsigned int frameNumber)
{
	// note that agents is an alias (using the &)
	const std::vector< SteerLib::AgentInterface * > & agents = _engine->getAgents();

	// for HybridAI
	std::vector< SteerLib::AgentInterface * > * tmp_agents = new std::vector< SteerLib::AgentInterface * >();
	for (int a=0; a < agents.size(); a++)
	{
		SteerLib::AgentInterface * tmp_agent = dynamic_cast<ReplayAgent *>(agents.at(a));
		if ( tmp_agent != NULL )
		{
			tmp_agents->push_back(tmp_agent);
		}
	}


	for (unsigned int i=0;  i < tmp_agents->size(); i++)
	{
		/// @todo
		///   The next version of the RecFileReader should also return an AgentGoalInfo struct, and this indirection should be unnecessary.
		AgentGoalInfo newGoal;
		newGoal.targetLocation = _simulationReader->getAgentGoalAtTime(i,(float)_currentTimeToPlayback);
		ReplayAgent * agent;
		agent = dynamic_cast<ReplayAgent*>(tmp_agents->at(i));
		Util::AxisAlignedBox oldBounds(agent->position().x-agent->radius(),
								agent->position().x+agent->radius(), 0.0f, 0.5f,
								agent->position().z-agent->radius(), agent->position().z+agent->radius());
		Util::Point oldLoc = agent->position();

	    agent->setPosition(_simulationReader->getAgentLocationAtTime(i,(float)_currentTimeToPlayback));
		agent->setForward(_simulationReader->getAgentOrientationAtTime(i,(float)_currentTimeToPlayback));
		agent->setEnabled(_simulationReader->isAgentEnabledAtTime(i,(float)_currentTimeToPlayback));
		agent->setRadius(_simulationReader->getAgentRadiusAtTime(i,(float)_currentTimeToPlayback));
		agent->setCurrentGoal(newGoal);

		// Somewhat good approximation of
		agent->setVelocity((oldLoc-(agent->position())).length()/dt * agent->forward());
		Util::AxisAlignedBox newBounds(agent->position().x-agent->radius(),
						agent->position().x+agent->radius(), 0.0f, 0.5f,
						agent->position().z-agent->radius(), agent->position().z+agent->radius());

		_engine->getSpatialDatabase()->updateObject( dynamic_cast<SpatialDatabaseItemPtr>(agent), oldBounds, newBounds);

		// save frame numer for first agent completion
		if ((frameNumber > 2) && !agent->enabled() && (_firstAgentCompletionFrame == -1) && !(_currentTimeToPlayback > _simulationStopTime))
		{
			_firstAgentCompletionFrame = frameNumber + 1;
			std::cout << "First agent completed at frame: " << _firstAgentCompletionFrame << std::endl;

			if (_dataFileName != "")
			{
				saveSimulationStats();
			}
		}
	}

	_currentTimeToPlayback += _fixedTimeStep * _playbackSpeed;
	if (_currentTimeToPlayback < _simulationStartTime) _currentTimeToPlayback = _simulationStartTime;
	if (_currentTimeToPlayback > _simulationStopTime) {
		_currentTimeToPlayback = _simulationStopTime;
		_engine->getEngineController()->pauseSimulation();
	}
}



void ReplayAgent::draw()
{
#ifdef ENABLE_GUI
	DrawLib::drawAgentDisc(_position, _radius, gBlue);
	// DrawLib::drawAgentDisc(_position, _forward, _radius, gGreen);

	/*
	if (_currentGoal.goalType == SteerLib::GOAL_TYPE_SEEK_STATIC_TARGET) {
			Util::DrawLib::drawFlag( this->currentGoal().targetLocation, Color(0.5f,0.8f,0), 2);
		}
	*/

#endif
}

void RecFilePlayerModule::saveSimulationStats()
{
	std::cout << "Rec file: " << _recFilename << std::endl;
	std::cout << "Storing simulation stats at: " << _dataFileName << std::endl;

	Logger * simulationLogger = LogManager::getInstance()->createLogger(_dataFileName, LoggerType::BASIC_APPEND);

	// adding data fields
	simulationLogger->addDataField("testcase|rec_file", DataType::String);
	simulationLogger->addDataField("frame", DataType::Integer);

	// adding field values

	LogObject simulationLogObject;

	DataItem d2;
	d2.string = _recFilename;
	simulationLogObject.addLogDataItem(d2);

	DataItem d1;
	d1.integerData = _firstAgentCompletionFrame;
	simulationLogObject.addLogDataItem(d1);

	/*
	if ((_saveTestcaseName && _saveFirstAgentCompletionFrame) || !(_saveTestcaseName && _saveFirstAgentCompletionFrame))
	{
		DataItem d1;
		d1.integerData = _firstAgentCompletionFrame;
		simulationLogObject.addLogDataItem(d1);

		DataItem d2;
		d2.string = _recFilename;
		simulationLogObject.addLogDataItem(d2);
	}
	else if (_saveTestcaseName)
	{
		DataItem d2;
		d2.string = _recFilename;
		simulationLogObject.addLogDataItem(d2);
	}
	else if (_saveFirstAgentCompletionFrame)
	{
		DataItem d1;
		d1.integerData = _firstAgentCompletionFrame;
		simulationLogObject.addLogDataItem(d1);
	}
	*/

	// writing to stream
	simulationLogger->writeLogObjectPretty(simulationLogObject);

	// cleaning up log data 
	simulationLogger->closeLog();
}