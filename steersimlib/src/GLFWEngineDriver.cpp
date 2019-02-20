//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//
//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//
//
// Copyright (c) 2009-2010 Shawn Singh, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file GLFWEngineDriver.cpp
/// @brief Implements the GLFWEngineDriver functionality.
///
/// @todo
///   - update documentation in this file
///

#ifdef ENABLE_GUI
#ifdef ENABLE_GLFW

#include <iostream>
//#include <GL/glew.h>
#include "SteerLib.h"
#include "core/GLFWEngineDriver.h"

#include "glm/glm/gtc/matrix_transform.hpp"

#define GLFWCALL

using namespace std;
using namespace SteerLib;
using namespace Util;



//
// callback wrappers for GLFW
//
static void GLFWCALL processFramebufferResizedEvent(GLFWwindow *w, int width, int height) { GLFWEngineDriver::getInstance()->processFramebufferResizedEvent(width, height); }
static void GLFWCALL processWindowResizedEvent(GLFWwindow *w, int width, int height) { GLFWEngineDriver::getInstance()->processWindowResizedEvent(width, height); }
static void GLFWCALL processKeyPressEvent(GLFWwindow *window, int key, int scancode, int action, int mods) { GLFWEngineDriver::getInstance()->processKeyPressEvent(window, key, scancode, action, mods); }
static void GLFWCALL processMouseButtonEvent(GLFWwindow *window, int button, int action, int mods) { GLFWEngineDriver::getInstance()->processMouseButtonEvent(window, button, action); }
static void GLFWCALL processMouseMovementEvent(GLFWwindow *window, double x, double y) { GLFWEngineDriver::getInstance()->processMouseMovementEvent(x, y); }
static void GLFWCALL processScrollEvent(GLFWwindow *window, double x, double y) { GLFWEngineDriver::getInstance()->processScrollEvent(x,y); }
static void GLFWCALL processWindowCloseEvent(GLFWwindow *w) {  GLFWEngineDriver::getInstance()->processWindowCloseEvent(); }


//
// getInstance()
//
// Singleton trick:  with the static instance in this function, we are guaranteed that the 
// constructor will be called before the instance is ever retrieved.
//
GLFWEngineDriver * GLFWEngineDriver::getInstance()
{
	static GLFWEngineDriver * singletonInstance = new GLFWEngineDriver();
	return singletonInstance;
}


//
// constructor
//
GLFWEngineDriver::GLFWEngineDriver()
{
	// Note that many of these values will be changed during init() anyway
	_alreadyInitialized = false;
	_engine = NULL;
	_mouseX = 0;
	_mouseY = 0;
	_wheelOffset = 0.0;
	_moveCameraOnMouseMotion = false;
	_rotateCameraOnMouseMotion = false;
	_zoomCameraOnMouseMotion = false;
	_multisampleAntialiasingSupported = false; // until we find out later
	_agentNearestToMouse = NULL;
	_nextScreenshotNumber = 0;
	_dumpFrames = false;
	_done = false;

	_useAntialiasing = true;
	_canUseMouseToSelectAgents = true;
	_canUseMouseWheelZoom = true;

	_options = NULL;
}


//
// init()
//
void GLFWEngineDriver::init(SimulationOptions * options)
{
	if (_alreadyInitialized) {
		throw GenericException("GLFWEngineDriver::init() was called twice, but it should only be called once.");
	}

	_options = options;

	std::cout << options->engineOptions.frameDumpDirectory << " " << std::endl;

	_alreadyInitialized = true;
	_done = false;
	_dumpFrames = false;

	_engine = new SimulationEngine();
	_engine->init(_options, this);

	_frameSaver = new Util::FrameSaver();
	// really just allows you to save multiple picutres without overwriting.
	_frameSaver->StartRecord(1024);
	if (_options->engineOptions.frameDumpDirectory != "")
	{
		// std::cout << options->engineOptions.frameDumpDirectory << " " << std::endl;
		_frameSaver->filePath = options->engineOptions.frameDumpDirectory;
	}

	_useAntialiasing = _options->guiOptions.useAntialiasing;
	_canUseMouseToSelectAgents = _options->guiOptions.canUseMouseSelection;
	_canUseMouseWheelZoom = _options->guiOptions.canUseMouseWheelZoom;
	_paused = _options->glfwEngineDriverOptions.pausedOnStart;

	_initGLFW();
	_checkGLCapabilities();
	checkError() ;
	_initGL(); // calls _engine->initGL() among other things...
	checkError() ;

	DrawLib::init();
	checkError() ;
	GLint range[2];
	//glGetIntegerv(GL_ALIASED_LINE_WIDTH_RANGE, range);
	//printf("AL %d - %d \n", range[0], range[1]) ;
	//glGetIntegerv(GL_SMOOTH_LINE_WIDTH_RANGE, range);
	//printf("SL %d - %d \n", range[0], range[1]) ;
}

void GLFWEngineDriver::finish()
{
	_engine->finish();
	DrawLib::finished();

	delete _engine;
	_engine = NULL;

	glfwTerminate();
	_alreadyInitialized = false;
	_engine = NULL;
	_mouseX = 0;
	_mouseY = 0;
	_wheelOffset = 0;
	_moveCameraOnMouseMotion = false;
	_rotateCameraOnMouseMotion = false;
	_zoomCameraOnMouseMotion = false;
	_multisampleAntialiasingSupported = false; // until we find out later
	_agentNearestToMouse = NULL;
	_nextScreenshotNumber = 0;
	_dumpFrames = false;
	_done = false;

	_useAntialiasing = true;
	_canUseMouseToSelectAgents = true;
	_canUseMouseWheelZoom = true;

	_options = NULL;
}

//
// Gets metric data as a string
//
const char * GLFWEngineDriver::getData()
{
	ModuleInterface * moduleInterface = (_engine->getModule("scenario"));
	ModuleInterface * aimoduleInterface = (_engine->getModule("sfAI"));
	std::string out;
	char * outptr;
	if ( moduleInterface != NULL && (aimoduleInterface != NULL) )
	{
		std::string out1 = moduleInterface->getData();
		std::string out2 = aimoduleInterface->getData();

		std::vector<std::string> vout1 = split(out1, '\n');
		std::vector<std::string> vout2 = split(out2, '\n');

		for (unsigned int i =0; i < vout1.size(); i++)
		{
			// I think the space is neccessary to sperate item
			// consider changing to another delimiter
			out = out + vout1.at(i) + " " + vout2.at(i) + "\n";
		}

		outptr = (char *) malloc(sizeof(char) * out.length());
		strncpy(outptr, out.c_str(), out.length());
		return outptr;
	}
	else
	{
		char * out = (char *) malloc(sizeof(char)*20);
		strncpy(out, "EXIT_SUCCESS_", 20);
		return out;
	}
}

LogData * GLFWEngineDriver::getLogData()
{
	ModuleInterface * moduleInterface = (_engine->getModule("scenario"));
	LogData * lD = moduleInterface->getLogData();
	ModuleInterface * aimoduleInterface = (_engine->getModule("rvo2dAI")); // TODO support all steering algorithms

	// TODO use this properly instead.
	std::vector<SteerLib::ModuleInterface*> modules = _engine->getAllModules();

	if ( (aimoduleInterface != NULL) )
	{
		lD->appendLogData(aimoduleInterface->getLogData());
	}
	return lD;
}

// Function that prints OpenGL context information
void printOpenGLVersion(GLFWwindow *window)
{
    // obtain the OpenGL version information
	const GLubyte* openGLversion = glGetString(GL_VERSION);
	const GLubyte* openGLvendor = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION) ;
    
    int major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR) ;
    int minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR) ;
        
	GLint texSize;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texSize);
    
	
    printf("OpenGL Vendor: %s\n", openGLvendor ) ;
    printf("Renderer: %s\n",  renderer) ;
    printf("OpenGL Version (string): %s\n",  openGLversion) ;
    printf("OpenGL Version (integer) : %d.%d\n", major, minor) ;
    printf("GLSL Version: %s\n", glslVersion) ;
    printf("Maximum Texture Size %d\n", texSize) ;
}

// Used during initialization of GLFW
static void error_callback(int error, const char* description)
{
	std::cerr << description;

}

void GLFWEngineDriver::_initGLFW()
{

	glfwSetErrorCallback(error_callback);

	// initialize glfw
	if (glfwInit() == GL_FALSE) {
		throw GenericException("Initializing GLFW failed.\n");
	}
	_options->glfwEngineDriverOptions.stereoMode = "off";

	glfwDefaultWindowHints(); // reset the hints

	// Open a window and create its OpenGL context
#ifdef __APPLE__ // 4.1 for the current OSX
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else //  for windows and linux
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	
	// specify some configuration that needs to happen before creating the window
	glfwWindowHint(GLFW_SAMPLES, 4);

	// use specified to use quadbuffer rendering
	if (_options->glfwEngineDriverOptions.stereoMode == "quadbuffer")
	{
		// check if we have support for it
		GLboolean stereoSupported = GL_TRUE;
		glGetBooleanv(GL_STEREO, &stereoSupported);

		if (stereoSupported == GL_TRUE)
			glfwWindowHint(GLFW_STEREO, GL_TRUE);
		else
		{
			std::cerr << "Quadbuffer rendering requested, but no support found on graphics card. Falling back to non-stereoscopic rendering." << std::endl;

			_options->glfwEngineDriverOptions.stereoMode = "off";
		}
	}

	// set the appropriate windowing mode
	
	//int mode = (_options->glfwEngineDriverOptions.fullscreen ? FULL_SCREEN : WINDOW);

	
	if (false) {
		_window = glfwCreateWindow(_options->glfwEngineDriverOptions.windowSizeX, _options->glfwEngineDriverOptions.windowSizeY, "My Title", glfwGetPrimaryMonitor(), NULL);
	}
	else {
		// create the glfw window
		glfwWindowHint(GLFW_RED_BITS, 8);
		glfwWindowHint(GLFW_GREEN_BITS, 8);
		glfwWindowHint(GLFW_BLUE_BITS, 8);
		glfwWindowHint(GLFW_STENCIL_BITS, 8);
		glfwWindowHint(GLFW_DEPTH_BITS, 8);

		_window = glfwCreateWindow(_options->glfwEngineDriverOptions.windowSizeX, _options->glfwEngineDriverOptions.windowSizeY, "My Title", NULL, NULL);
	}
	
	if (_window == NULL)
	{
		throw GenericException("Could not open a window using glfwOpenWindow().");
	}

	glfwMakeContextCurrent(_window);

	// specify some configuration that needs to happen after creating the window
	glfwSetWindowTitle(_window, _options->glfwEngineDriverOptions.windowTitle.c_str() );
	glfwSetWindowPos(_window, _options->glfwEngineDriverOptions.windowPositionX, _options->glfwEngineDriverOptions.windowPositionY);
	
	
	int interval = _options->guiOptions.useVsync ? 1 : 0;
	glfwSwapInterval(interval);

	
	//glfwSwapInterval(0) ;
	checkError();

	// Load the OpenGL function pointers
	glewExperimental = GL_TRUE;
	GLenum errGlew = glewInit();
	if (GLEW_OK != errGlew)
	{
		std::cerr << "ERROR: Cannot initialize glew\n";
		fprintf(stderr, "Error: %s\n", glewGetErrorString(errGlew));
		glfwDestroyWindow(_window);
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	checkError();

	printOpenGLVersion(_window);

	// register GLFW callbacks
	// note the "::" is needed to resolve the static global functions, not the GLFWEngineDriver member functions.
	glfwSetFramebufferSizeCallback(_window, ::processFramebufferResizedEvent);
	glfwSetWindowSizeCallback(_window, ::processWindowResizedEvent );
	glfwSetKeyCallback(_window, ::processKeyPressEvent );

	glfwSetMouseButtonCallback(_window, ::processMouseButtonEvent );
	glfwSetCursorPosCallback(_window,  ::processMouseMovementEvent );
	glfwSetScrollCallback(_window,  ::processScrollEvent );
	glfwSetWindowCloseCallback(_window, ::processWindowCloseEvent );

	

	int w, h;
	glfwGetWindowSize(_window, &w, &h);
	processWindowResizedEvent(w, h);
	glfwGetFramebufferSize(_window, &w, &h);
	processFramebufferResizedEvent(w, h);
	checkError();

}

/// @todo 
///   - need to properly initialize the antialiasing mode based on user options
///     and initialize _useAntialising at the same time.
///   - figure out how to approrpiately toggle vsync
void GLFWEngineDriver::_checkGLCapabilities()
{
	int p;
	glGetIntegerv(GL_SAMPLE_BUFFERS, &p);
	if (p == 1) {
		_multisampleAntialiasingSupported = true;
	}

	else {
		_multisampleAntialiasingSupported = false;
	}


}

//
// initGL()
//
void GLFWEngineDriver::_initGL(void)
{
	
	if (_multisampleAntialiasingSupported && _useAntialiasing) {
		glEnable( GL_MULTISAMPLE );
	} else {
		glDisable( GL_MULTISAMPLE );
	}
	
	checkError();
	_engine->initGL();
	checkError();

}

void GLFWEngineDriver::loadSimulation()
{

	#ifdef _DEBUG1
	bool verbose = true;  // TODO: make this a user option...
	#else
	bool verbose = false;
	#endif
	if (verbose) std::cout << "\rInitializing...\n";
	_engine->initializeSimulation();

	if (verbose) std::cout << "\rPreprocessing...\n";
	_engine->preprocessSimulation();
}

void GLFWEngineDriver::unloadSimulation()
{
	#ifdef _DEBUG1
	bool verbose = true;  // TODO: make this a user option...
	#else
	bool verbose = false;
	#endif

	if (verbose) std::cout << "\rPostprocessing...\n";
	_engine->postprocessSimulation();

	if (verbose) std::cout << "\rCleaning up...\n";
	_engine->cleanupSimulation();

	if (verbose) std::cout << "\rDone.\n";
}

void GLFWEngineDriver::startSimulation()
{
	#ifdef _DEBUG1
	bool verbose = true;  // TODO: make this a user option...
	#else
	bool verbose = false;
	#endif
	bool done = false;
	// loop until the engine tells us its done
	if (verbose) std::cout << "\rSimulation is running...\n";
	while (!_done) {

		// Finding the agent closest to mouse uses an exhaustive search across all agents.
		// the algorithm could probably be better (i.e. use the spatial database to find it)
		// but its not (yet) worth implementing that way.  Just change DEFAULT_CAN_USE_MOUSE_SELECTION to
		// false if you want to disable it.
		if (_canUseMouseToSelectAgents) {
			_findClosestAgentToMouse();
		}

		// Update the AI.
		if (_engine->update(_paused) == false) {
			// The engine indicated the simulaton should finish
			_done = true;
		}
		else {
			// The simulation is continuing, so draw everything using openGL.
			_drawScene();
		}

		// std::cout << getEngine()->getClock().getRealFps() << " fps" << std::endl;
		// sprintf( titlestr, "SteerSuite (%.1f FPS)", getRealFps() );
		std::ostringstream stream;
		stream << "SteerSuite: " << getEngine()->getClock().getRealFps() << " fps";
		std::string fpsString = stream.str();

		// Convert the new window title to a c_str and set it
		const char* pszConstString = fpsString.c_str();
		glfwSetWindowTitle(_window,pszConstString);
		// glfwSetWindowTitle( getEngine()->getClock().getRealFps() << " fps" );
	}

	if (verbose) std::cout << "\rFrame Number:   " << _engine->getClock().getCurrentFrameNumber() << std::endl;
}

void GLFWEngineDriver::stopSimulation()
{
	// TODO not sure what to do here yet, seems like it is meant for multi-threading
	throw Util::GenericException("CommandLineEngineDriver does not support stopSimulation().");
}

//
// run() - returns if the simulation is done, but there are other ways the program will exit (e.g. user closes the window)
//
void GLFWEngineDriver::run()
{
	bool verbose = true;  // TODO: make this a user option ??.

	if (verbose) std::cout << "\rInitializing...\n";
	_engine->initializeSimulation();

	if (verbose) std::cout << "\rPreprocessing...\n";
	_engine->preprocessSimulation();

	if (verbose) std::cout << "\rSimulation is running...\n";
	while (!_done) {

		// Finding the agent closest to mouse uses an exhaustive search across all agents.
		// the algorithm could probably be better (i.e. use the spatial database to find it)
		// but its not (yet) worth implementing that way.  Just change DEFAULT_CAN_USE_MOUSE_SELECTION to 
		// false if you want to disable it.
		if (_canUseMouseToSelectAgents) {
			_findClosestAgentToMouse();
		}

		// Update the AI.
		if (_engine->update(_paused) == false) {
			// The engine indicated the simulaton should finish
			_done = true;
		}
		else {
			// The simulation is continuing, so draw everything using openGL.
			_drawScene();
		}

		// std::cout << getEngine()->getClock().getRealFps() << " fps" << std::endl;
		// sprintf( titlestr, "SteerSuite (%.1f FPS)", getRealFps() );
		std::ostringstream stream;
		stream << "SteerSuite: " << getEngine()->getClock().getRealFps() << " fps";
		std::string fpsString = stream.str();

		// Convert the new window title to a c_str and set it
		const char* pszConstString = fpsString.c_str();
		glfwSetWindowTitle(_window, pszConstString);
		// glfwSetWindowTitle( getEngine()->getClock().getRealFps() << " fps" );

		// process events
		glfwPollEvents();
	}

	if (verbose) std::cout << "\rPostprocessing...\n";
	_engine->postprocessSimulation();

	if (verbose) std::cout << "\rCleaning up...\n";
	_engine->cleanupSimulation();

	if (verbose) std::cout << "\rDone.\n";
}

void GLFWEngineDriver::_drawScene()
{
	// get the camera from the engine
	Camera & cam = _engine->getCamera();

	// clear color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (_options->glfwEngineDriverOptions.stereoMode == "off")
	{
		cam.apply();
		DrawLib::positionLights();
		_engine->draw();
	}
	else
	{
		cerr << "============ STEREO NOT UDPATED YET !!!!!!!!!!!============\n";
		/***********************
		// NOTE: going to clean up this code soon...

		// method used: parallel axis asymmetric frustum perspective projection
		// url: ftp://ftp.sgi.com/opengl/contrib/kschwarz/GLUT_INTRO/SOURCE/PBOURKE/index.html
		const float eyeSep = 0.3f; // TODO: make configurable?
		const float focallength = 100.0f; // TODO: make configurable?
		const float near_ = 0.1;
		const float far_ = 4000;
		const float fov = cam.fovy();

		float ratio = _options->glfwEngineDriverOptions.windowSizeX / (double)_options->glfwEngineDriverOptions.windowSizeY;
		float hh = near_ * tanf(0.0174532925199432957692369076849f * fov / 2); // half height; constant is pi / 180
		float ndfl = near_ / focallength;

		// setup the left view
		if (_options->glfwEngineDriverOptions.stereoMode == "side-by-side")
			glViewport(0, 0, _options->glfwEngineDriverOptions.windowSizeX / 2, _options->glfwEngineDriverOptions.windowSizeY);
		else if (_options->glfwEngineDriverOptions.stereoMode == "top-and-bottom")
			glViewport(0, 0, _options->glfwEngineDriverOptions.windowSizeX, _options->glfwEngineDriverOptions.windowSizeY / 2);
		else if (_options->glfwEngineDriverOptions.stereoMode == "quadbuffer")
			glViewport(0, 0, _options->glfwEngineDriverOptions.windowSizeX, _options->glfwEngineDriverOptions.windowSizeY);

		//if (_options->glfwEngineDriverOptions.stereoMode == "quadbuffer")
		//{
		// clear left and right buffers
		//	glDrawBuffer(GL_BACK_LEFT);
		//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//	glDrawBuffer(GL_BACK_RIGHT);
		//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//}

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		float left = -ratio * hh - 0.5 * eyeSep * ndfl;
		float right = ratio * hh - 0.5 * eyeSep * ndfl;
		float top = hh;
		float bottom = -hh;
		glFrustum(left, right, bottom, top, near_, far_);

		if (_options->glfwEngineDriverOptions.stereoMode == "quadbuffer")
			glDrawBuffer(GL_BACK_LEFT);

		cam.apply_stereo(false);
		DrawLib::positionLights();
		_engine->draw();

		if (_options->glfwEngineDriverOptions.stereoMode == "side-by-side")
			glViewport(_options->glfwEngineDriverOptions.windowSizeX / 2, 0, _options->glfwEngineDriverOptions.windowSizeX / 2, _options->glfwEngineDriverOptions.windowSizeY);
		else if (_options->glfwEngineDriverOptions.stereoMode == "top-and-bottom")
			glViewport(0, _options->glfwEngineDriverOptions.windowSizeY / 2, _options->glfwEngineDriverOptions.windowSizeX, _options->glfwEngineDriverOptions.windowSizeY / 2);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		left = -ratio * hh + 0.5 * eyeSep * ndfl;
		right = ratio * hh + 0.5 * eyeSep * ndfl;
		top = hh;
		bottom = -hh;
		glFrustum(left, right, bottom, top, near_, far_);

		if (_options->glfwEngineDriverOptions.stereoMode == "quadbuffer")
			glDrawBuffer(GL_BACK_RIGHT);

		cam.apply_stereo(true);
		DrawLib::positionLights();
		_engine->draw();
		*********************************/
	}

	// check for errors
	//int error = checkErrors();
	//if (error != GL_NO_ERROR)
	//{
	//	std::cerr << "An OpenGL error occurred: " << gluErrorString(error) << "\n";
	//	throw GenericException("OpenGL error occurred in GLFWEngineDriver::_drawScene().");
	//}
	if (_dumpFrames)
		writeScreenCapture();

	// double buffering, swap back and front buffers
	glfwSwapBuffers(_window);
}


//
// drawGUI() - draw any GUI elements or global GUI annotations that have nothing to do with the simulation.
//
void GLFWEngineDriver::_drawGUI()
{
	if (_agentNearestToMouse != NULL) DrawLib::drawHighlight(_agentNearestToMouse->position(), _agentNearestToMouse->forward(), _agentNearestToMouse->radius());
}



void GLFWEngineDriver::writeScreenCapture()
{
	// cerr << "WARNING: no screenshot taken, feature not implemented for GLFW yet.\n";
	// _frameSaver->filePath = "./";
	// _frameSaver->baseName = "frame";

	_frameSaver->DumpPPM(_options->glfwEngineDriverOptions.windowSizeX,
			_options->glfwEngineDriverOptions.windowSizeY);

}

void GLFWEngineDriver::dumpTestCase()
{
	std::vector<SteerLib::AgentInitialConditions> _agents;
	// std::vector<SteerLib::BoxObstacle> _obstacles;
	std::vector<SteerLib::ObstacleInterface*> _obstacles;
	int j;

	for (j=0; j < _engine->getAgents().size(); j++)
	{
		_agents.push_back(AgentInterface::getAgentConditions(_engine->getAgents().at(j)));
	}

	for(set<SteerLib::ObstacleInterface*>::const_iterator iter = _engine->getObstacles().begin(); iter != _engine->getObstacles().end(); iter++)
	{
		_obstacles.push_back( (*iter) );
	}

	SteerLib::TestCaseWriter testCaseWriter;
	//TODO should update filename to change or be user inputed.
	testCaseWriter.writeTestCaseToFile("test",_agents,_obstacles,_engine);
}



void GLFWEngineDriver::_findClosestAgentToMouse()
{
	// gotta find mouse/screen position in world coords

	// Since screen is 2d and world coords is 3d, we convert screen
	// position into a ray originating from the camera position and
	// going in the direction indicated by the mouse pos

	// next bit is apparently standard openGl procedure to getting what
	// we want:

	int viewport[4];

	double x1, y1, z1, x2, y2, z2;

	//glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
	//glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);
	glm::mat4  modelViewMat = DrawLib::getModelViewMat();
	glm::mat4 projectionMat = DrawLib::getProjectionMat();

	glm::vec4 viewportf( (float) viewport[0], (float) viewport[1], (float) viewport[2], (float) viewport[3]);
	int x = _mouseX;
	int y = viewport[3] - _mouseY;
	//gluUnProject(x, y, 0, modelView, projection, viewport, &x1, &y1, &z1);
	//gluUnProject(x, y, 1, modelView, projection, viewport, &x2, &y2, &z2);

	glm::vec3 p1 = glm::unProject(glm::vec3((float) x, (float) y, 0.f), modelViewMat, projectionMat, viewportf);
	glm::vec3 p2 = glm::unProject(glm::vec3((float) x, (float) y, 1.f), modelViewMat, projectionMat, viewportf);
	x1 = p1[0];
	y1 = p1[1];
	z1 = p1[2];

	x2 = p2[0];
	y2 = p2[1];
	z2 = p2[2];


	// Our ray: originates at the camara and goes somewhere
	Vector cameraRay = Vector((float)(x2-x1), (float)(y2-y1), (float)(z2-z1));
	Point cameraPos = _engine->getCamera().position();

	// to determine which agents are closest to the mouse click, figure out which
	// agents are closest to the ray
	// NOTE: assuming that agents are all at the y=0 plane.
	float t = -cameraPos.y / cameraRay.y;  // this gives us the t value for where the point along the ray would be on the y=0 plane.
	Point locationOfMouseOnYPlane = cameraPos + t*cameraRay;

	// nearest: nearest agent to mouse pos
	// nearestAndContained: nearest agent to mouse pos and the point mouse is within radius

	AgentInterface* nearest = NULL;

	float distanceToNearest = FLT_MAX;

	const std::vector<AgentInterface*> & agents = _engine->getAgents();
	for(std::vector<AgentInterface*>::const_iterator i = agents.begin(); i != agents.end(); ++i)
	{
		float dist = distanceBetween((*i)->position(), locationOfMouseOnYPlane);
		// if its the closest one, but also within some distance threshold
		if ((dist < (*i)->radius()+0.5f) && (dist < distanceToNearest)) {
			nearest = (*i);
			distanceToNearest = dist;
		}
	}
	_agentNearestToMouse = nearest;

}


//=====================================================
// GLUT callback functions
//=====================================================


void GLFWEngineDriver::processWindowResizedEvent(int width, int height)
{
	//_engine->resizeGL(width, height);
	// This is in screen coordinates not pixel coordinates that glViewPort expects.
	// Use FramebufferResizedEvent instead
	return;
}

void GLFWEngineDriver::processFramebufferResizedEvent(int width, int height)
{
	_engine->resizeGL(width, height);
}

void GLFWEngineDriver::processKeyPressEvent(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if ((key == _options->keyboardBindings.quit) && (action==GLFW_PRESS)) {
		_done = true;
	}
	else if ((key == _options->keyboardBindings.toggleAntialiasing) && (action==GLFW_PRESS)) {
		if (!_multisampleAntialiasingSupported) {
			cerr << "WARNING: toggling antialiasing may have no effect; antialiasing does not seem to be supported.\n";
		}
		_useAntialiasing = !_useAntialiasing;
		if (_useAntialiasing) {
			glEnable( GL_MULTISAMPLE );
		} else {
			glDisable( GL_MULTISAMPLE );
		}
		std::cout << "Antialiasing turned " << (_useAntialiasing ? "on" : "off") << std::endl;
	}
	else if ((key == _options->keyboardBindings.printCameraInfo) && (action==GLFW_PRESS)) {
		cout << "CAMERA INFO:" << endl;
		cout << "  Position:     " << _engine->getCamera().position() << endl;
		cout << "  LookAt:       " << _engine->getCamera().lookat() << endl;
		cout << "  Up:           " << _engine->getCamera().up() << endl;
	} 
	else if ((key == _options->keyboardBindings.stepForward) && (action==GLFW_PRESS)) {
		pauseAndStepOneFrame();
	}
	else if ((key == _options->keyboardBindings.pause) && (action==GLFW_PRESS)) {
		togglePausedState();
	}
	else if ((key == (int)'F') && (action==GLFW_PRESS)) {
		cout << "Frame Number " << _engine->getClock().getCurrentFrameNumber() << "\n";
	}
	else if ((key == _options->keyboardBindings.takeScreenshot) && (action==GLFW_PRESS))
	{
		writeScreenCapture();
	}
	else if ((key == _options->keyboardBindings.dumpTestCase) && (action==GLFW_PRESS))
	{
		std::cout << "Dumping testcase of current simulation" << std::endl;
		dumpTestCase();
	}
	else if ((key == _options->keyboardBindings.startDumpingFrames) && (action==GLFW_PRESS))
	{
		// home button
		std::cout << "Saving frames" << std::endl;
		_dumpFrames = true;
	}
	else if ((key == _options->keyboardBindings.stopDumpingFrames) && (action==GLFW_PRESS))
	{
		// end button
		std::cout << "Ending frame saving" << std::endl;
		_dumpFrames = false;
	}
	else {
		if (_engine) _engine->processKeyboardInput( key, action);
	}
}


void GLFWEngineDriver::processMouseButtonEvent(GLFWwindow *window, int button, int action)
{
	// set the position of the cursor ---- Added by Petros May 23, 2017
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	_mouseX = xpos ;
	_mouseY = ypos ;
	
	bool controlKeyPressed = (  (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)==GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL)==GLFW_PRESS) );

	if ((button ==  _options->mouseBindings.selectAgent) && (action == GLFW_PRESS)) {
		if(!controlKeyPressed) {
			if (_agentNearestToMouse != NULL) {
				if (!_engine->isAgentSelected(_agentNearestToMouse)) {
					_engine->selectAgent(_agentNearestToMouse);
					unsigned int i;
					for (i=0; i< _engine->getAgents().size(); i++) {
						if ( _engine->getAgents()[i] == _agentNearestToMouse ) {
							break;
						}
					}
					if (_agentNearestToMouse != NULL) cerr << "selected agent #" << i << " at location " << _agentNearestToMouse->position() <<
							" agent mem loc " << _agentNearestToMouse << " total " << _engine->getSelectedAgents().size() << " agents are currently selected)." << endl;
				}
				else {
					_engine->unselectAgent(_agentNearestToMouse);
					unsigned int i;
					for (i=0; i< _engine->getAgents().size(); i++) {
						if ( _engine->getAgents()[i] == _agentNearestToMouse ) {
							break;
						}
					}
					if (_agentNearestToMouse != NULL) cerr << "un-selected agent #" << i << " (total " << _engine->getSelectedAgents().size() << " agents are currently selected)." << endl;
				}
			}
			else {
				_engine->unselectAllAgents();
			}
		}
	}

	if ((button == _options->mouseBindings.moveCamera) && (action == GLFW_PRESS)) {
		/*if(controlKeyPressed)*/ _moveCameraOnMouseMotion = true;
	}

	if ((button == _options->mouseBindings.moveCamera) && (action == GLFW_RELEASE)) {
		_moveCameraOnMouseMotion = false;
	}

	if ((button == _options->mouseBindings.rotateCamera) && (action == GLFW_PRESS)) {
		if(controlKeyPressed) _rotateCameraOnMouseMotion = true;
	}

	if ((button == _options->mouseBindings.rotateCamera) && (action == GLFW_RELEASE)) {
		_rotateCameraOnMouseMotion = false;
	}

	if ((button == _options->mouseBindings.zoomCamera) && (action == GLFW_PRESS)) {
		if(controlKeyPressed) _zoomCameraOnMouseMotion = true;
	}

	if ((button == _options->mouseBindings.zoomCamera) && (action == GLFW_RELEASE)) {
		_zoomCameraOnMouseMotion = false;
	}

	_engine->processMouseButtonEvent(button, action);
	
	
}


void GLFWEngineDriver::processMouseMovementEvent(double x, double y)
{

	// get mouse changes
	int deltaX = x - _mouseX;
	int deltaY = y - _mouseY;
	
	// update mouse position
	_mouseX = x;
	_mouseY = y;

	// camera rotate
	if(_rotateCameraOnMouseMotion)
	{
		float xAdjust = -deltaX * _options->guiOptions.mouseRotationFactor;
		float yAdjust = deltaY * _options->guiOptions.mouseRotationFactor;

		_engine->getCamera().nudgeRotate(yAdjust, xAdjust);
	
	}

	// camera zoom
	if(_zoomCameraOnMouseMotion)
	{
		float yAdjust = deltaY * _options->guiOptions.mouseZoomFactor;
		_engine->getCamera().nudgeZoom(yAdjust);
	}

	// camera move
	if(_moveCameraOnMouseMotion)
	{
		float xAdjust = deltaX * _options->guiOptions.mouseMovementFactor;
		float yAdjust = deltaY * _options->guiOptions.mouseMovementFactor;

		_engine->getCamera().nudgePosition(xAdjust, yAdjust);
	}

	_engine->processMouseMovementEvent(deltaX, deltaY);
}

// A mouse wheel provides only y offsets
void GLFWEngineDriver::processScrollEvent(double xoffset, double yoffset)
{
	if (_canUseMouseWheelZoom) {
		double deltaWheel = yoffset ;
		_wheelOffset = yoffset;
		if (deltaWheel != 0.0) {
			float wheelAdjust = (float) 1.0 * deltaWheel * _options->guiOptions.mouseZoomFactor;
			_engine->getCamera().nudgeZoom(wheelAdjust);
		}
	}
}

int GLFWEngineDriver::processWindowCloseEvent()
{
	// allow the user to exit at any time
	_done = true;
	return GL_TRUE;
}

#endif // ifdef ENABLE_GLFW
#endif // ifdef ENABLE_GUI
