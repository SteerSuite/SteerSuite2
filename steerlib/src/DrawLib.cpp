//
// Copyright (c) 2009-2018 Brandon Haworth, Glen Berseth, Muhammad Usman, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//



/// @file DrawLib.cpp
/// @brief Implements Util::DrawLib functionality.
///

#ifdef ENABLE_GUI
#include <iostream>


#include "opengl.h"

// glm::vec3, glm::vec4, glm::ivec4, glm::mat4
#include "glm/glm/glm.hpp"
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtc/matrix_inverse.hpp"
// glm::value_ptr
#include "glm/glm/gtc/type_ptr.hpp"

#include "util/image.h"
#include "util/DrawLib.h"
#include "util/Geometry.h"
#include "util/GLgeomObjects.h"
#include "util/program.h"
#include "util/matStack.h"



#include <string>


using namespace Util;
using namespace std;

// The shader list
static Shaders gShaders;

// The canonical objects (primitives) ;

static Quad *gQuad;
static Quad *gQuadFixed;
static GLLine *gLine ;
static GLCircle *gCircle;
static Square *gSquare;
static Cube *gCube;
static Cylinder *gCylinder;
static Cone *gCone;
static Sphere *gSphere;
static GLStar *gStar;
static GLDisk *gDisk;
static GLTriangle *gTriangle;


static bool gUsingLighting = false;
static bool gUsingTextures = false;


static glm::mat4 gProjMat(1.0f);
static glm::mat4 gMVP(1.0f);

#ifdef WIN32
const std::string gProjectPath = "../../";
#else
const std::string gProjectPath = "../../";
//const std::string gProjectPath = "./" ;
#endif

const int MAX_STACK_SIZE = 100; // the maximum size of the modelview stack

// Path for shader;
const std::string gShaderDir = gProjectPath + "steerlib/src/shaders/";
// Path for saving frames
const std::string gFrameDir = gProjectPath + "frames/";


// The modeling matrix stack
static ModelviewStack gMS(MAX_STACK_SIZE); // the number is the maximum elements in the stack

const GLuint gNumTex = 3;
GLuint gTexIDs[gNumTex]; // Three textures


void DrawLib::setMaterial(glm::vec3 Ka, glm::vec3 Kd, glm::vec3 Ks, float a, float shininess)
{
	gShaders.setFloat3("Material.Ka", Ka[0], Ka[1], Ka[2]);
	gShaders.setFloat3("Material.Kd", Kd[0], Kd[1], Kd[2]);
	gShaders.setFloat3("Material.Ks", Ks[0], Ks[1], Ks[2]);
	gShaders.setFloat("Material.Shininess", shininess);
	gShaders.setFloat("Material.alpha", a);
}

void DrawLib::setLight(glm::vec4 Position, glm::vec3 Ia, glm::vec3 Id, glm::vec3 Is)
{
	gShaders.setFloat3("Light.Ia", Ia[0], Ia[1], Ia[2]);
	gShaders.setFloat3("Light.Id", Id[0], Id[1], Id[2]);
	gShaders.setFloat3("Light.Is", Is[0], Is[1], Is[2]);
	gShaders.setFloat4("Light.Position", Position[0], Position[1], Position[2], Position[3]);
}

void DrawLib::setColour(float c1, float c2, float c3)  {
	setColour(c1, c2, c3, 1.0f);
}

void DrawLib::setColour(float c1, float c2, float c3, float a)
{
	// Ks always one for non metalic materials
	glm::vec3 ks = glm::vec3(1.0f);
	glm::vec3 c(c1, c2, c3);
	setMaterial(c, c, 0.5f*ks, a, 40.0f);
}

// Tells the fragment shader which of the 3 textures to use and scales the text coordinates for tiling
// 0 means no texture
// 1,2,3 specifies the respective texture
// 12 means 1 and 2
// 23 means 2 and 3
// 13 means 1 and 3
// 123 means all three
// 100 menas use the per vertex colour as the texture
void DrawLib::useTexture(int v, float coordScale)
{
	checkError() ;
	gShaders.setInt("useTex", v);
	gShaders.setFloat("texCoordScale", coordScale);

	if (v) gUsingTextures = true;
	else gUsingTextures = false;

	checkError() ;
}

void DrawLib::useLighting(int t) // should be 0 or 1
{
	checkError() ;
	gShaders.setInt("useLighting", t);
	if (t) gUsingLighting = true;
	else gUsingLighting = false;
	checkError() ;
}

void DrawLib::setProjectionMatrix(glm::mat4 m) {
	gProjMat = m;
}

// Sets the Modelview, the normal, and the  MVP matrices from the stack given
void DrawLib::setMatricesFromStack(void)
{
	glm::mat4 m = gMS.Top();
	gShaders.setMatrix4f("modelviewMat", m);
	checkError();

	gMVP = gProjMat*m;
	gShaders.setMatrix4f("MVP", gMVP);

	glm::mat4 nm = glm::inverseTranspose(m);
	gShaders.setMatrix4f("normalMat", nm);
	checkError();

}


// Draws a unit sphere centered at the origin of radius 1.0 ;
void DrawLib::drawSphere(void)
{

	DrawLib::setMatricesFromStack();
	gSphere->Draw();

}



// Utility function that draws a 2x2x2 cube center at the origin
// Sets the modelview matrix at the "modelviewMat" uniform of gProg1
// and the corresponding "normalMat"
void DrawLib::drawCube(void)
{
	DrawLib::setMatricesFromStack();
	gCube->Draw();
}


// Utility function that draws a 1x1 quad center at the origin
// Sets the modelview matrix and the normal matrix of gProg1
void DrawLib::drawSquare(void)
{
	DrawLib::setMatricesFromStack();
	gSquare->Draw();

}

// Utility function that draws a cylinder along z of height 1 centered at the origin
// and radius 0.5 ;
// Sets the modelview matrix and the normal matrix of gProg1
void drawCylinder()
{

	DrawLib::setMatricesFromStack();
	gCylinder->Draw();

}

// Utility function that draws a cone along z of height 1 centered at the origin
// and base radius 1.0 ;
// Sets the modelview matrix and the normal matrix of gProg1
void DrawLib::drawCone(void)
{

	DrawLib::setMatricesFromStack();
	gCone->Draw();

}


// Draws a cubic bezier patch;
void DrawLib::drawBezierPatch(BezierPatch3 *b)
{
	DrawLib::setMatricesFromStack();
	b->Draw();
}



void initTexture()
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	if (0)
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	else {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

}

void setTexture(GLuint prog, std::string uname, GLuint activeTexID)
{
	initTexture();
	GLuint texUniform = glGetUniformLocation(prog, uname.c_str());
	if ((int)texUniform >= 0) {
		glUniform1i(texUniform, activeTexID);
	}
	else {
		std::cerr << "Shader did not contain the uniform " << uname << std::endl;
	}
}

void setTextures(void)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTexIDs[0]);
	setTexture(gShaders.getActiveID(), "texture1", 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gTexIDs[1]);
	setTexture(gShaders.getActiveID(), "texture2", 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gTexIDs[2]);
	setTexture(gShaders.getActiveID(), "texture3", 2);

}
void loadTextures(void)
{

	std::string fname = gProjectPath + "media/" + "Brick-2377.bmp";
	//char fname_char[200];
	//strcpy(fname_char, fname.c_str());
	GL_Image2D Img1((char *) fname.c_str());
	fname = gProjectPath + "media/" + "floor.bmp";
	//strcpy(fname_char, fname.c_str());
	GL_Image2D Img2((char *) fname.c_str());

	glGenTextures(gNumTex, gTexIDs);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTexIDs[0]);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Img1.m_width,
		Img1.m_height, 0, GL_RGB, GL_UNSIGNED_BYTE,
		Img1.m_data);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gTexIDs[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Img2.m_width,
		Img2.m_height, 0, GL_RGB, GL_UNSIGNED_BYTE,
		Img2.m_data);
	// setTexture(gShaders.getActiveID(),"texture2", 1) ;


	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gTexIDs[2]);
	int i, j, c;
	GLubyte checkImage[64][64][4];
	for (i = 0; i < 64; i++) {
		for (j = 0; j < 64; j++) {
			c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64,
		64, 0, GL_RGBA, GL_UNSIGNED_BYTE,
		checkImage);
}

// set the uniforms for the active shader
void DrawLib::initUniforms(void)
{
	checkError() ;
	setTextures();
	checkError() ;
	DrawLib::useLighting(0);
	checkError() ;
	DrawLib::useTexture(0,1.0);
	setMaterial(glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(1.f, 0.f, 0.f), glm::vec3(1.0f, 1.0f, 1.f),1.0f, 20.0);
	checkError() ;
	DrawLib::setLight(glm::vec4(0.f, 0.f, 0.f, 10.f), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	checkError() ;
}

void DrawLib::useShader(int index)
{
	
	checkError() ;
	gShaders.use(index);
	checkError() ;
	DrawLib::initUniforms();
	checkError() ;
}


/**
std::vector<GLuint> DrawLib::_displayLists;
int DrawLib::_currentDisplayListBeingWritten = -1;
int DrawLib::_agentDisplayList = -1;
int DrawLib::_agentDotDisplayList = -1;
int DrawLib::_flagDisplayList = -1;
int DrawLib::_cubeDisplayList = -1;
int DrawLib::_sphereDisplayList = -1;
int DrawLib::_cylinderDisplayList = -1;
GLUquadricObj * DrawLib::_quadric = NULL;
**/

bool DrawLib::_initialized = false;

void DrawLib::init() {

	if (_initialized) return;

	_initialized = true;

	checkError() ;
	// load textures
	loadTextures(); // Before loading the shaders

	checkError() ;
	// Load the shader pairs -- MUST BE DONE before the objects are loaded
	// so that the objects can retrieve the location of the attributes.
	// All programs should have the same attributes at the same locations.
	// First program is used for reference.

	gShaders.setShaderDir(gShaderDir);
	if (!gShaders.loadShaderPair("adsPerPixel")) exit(-1);
	//if (!gShaders.loadShaderPair("adsPerVertex")) exit(-1); // not reading the VertexTexCoord for some reason???

	// chose which shader to use
	checkError() ;
	useShader(0);

	checkError() ;

	// Load canonical primitives (objects)
	GLuint pid = gShaders.getActiveID();

	gLine = new GLLine("line1", pid);
	checkError();
	gQuad = new Quad("quadUpdate1", pid);
	checkError();
	gQuadFixed = new Quad("quadFixed1", pid);
	checkError();
	gSquare = new Square("square1", pid);
	checkError();
	gCube = new Cube("cube1", pid);
	checkError();
	gCylinder = new Cylinder("cylinder1", 20, pid);
	checkError();
	gCone = new Cone("cone1", 20, pid);
	checkError();
	gSphere = new Sphere("sphere1", 20, pid);
	checkError();
	gStar = new GLStar("star1", 15, pid);
	checkError();
	gCircle = new GLCircle("circle1", 20, pid);
	checkError();
	gDisk = new GLDisk("disk1", 20, pid);
	checkError();
	gTriangle = new GLTriangle("triangle1", pid);
	checkError();
	

	gMS.LoadIdentity(); // init matrix stack

	enableLights();
}

void DrawLib::finished()
{
	_initialized = false;

	delete gCircle;
	delete gQuad;
	delete gQuadFixed;
	delete gSquare;
	delete gCone;
	delete gSphere;
	delete gCube;
	delete gCylinder;
	delete gLine;
	delete gStar;
	delete gTriangle;
	delete gDisk;


}

//
// setBackgroundColor()
//
void DrawLib::setBackgroundColor(const Color& color)
{
    glClearColor(color.r, color.g, color.b, 1.0f);
}


//
// enableLights()
//
void DrawLib::enableLights()
{
	useLighting(1);
}


//
// disableLights()
//
void DrawLib::disableLights()
{
	useLighting(0);
}


//
// positionLights()
//
void DrawLib::positionLights()
{
	gShaders.setFloat4("Light.Position", 1.0f, 1.0f, 1.0f, 0.0f);
}



//
// drawLine()
//
void DrawLib::drawLine(const Point & startPoint, const Point & endPoint)
{
	Color color(0.f, 0.f, 0.f);
	drawLine(startPoint, endPoint, color);
}

void DrawLib::drawLine(const Point & startPoint, const Point & endPoint, const Color &color)
{
	float alpha = 1.0f;
	drawLineAlpha(startPoint, endPoint, color, alpha);
}

void DrawLib::drawLineAlpha(const Point & startPoint, const Point & endPoint, const Color &color, float alpha)
{
	DrawLib::setMatricesFromStack();
	gLine->Draw(startPoint, endPoint, color, alpha);
}


void DrawLib::drawLine(const Point & startPoint, const Point & endPoint, const Color &color, float thickness)
{
	DrawLib::setMatricesFromStack();
#ifndef __APPLE__
	glLineWidth(thickness);
#endif
	//drawLineAlpha(startPoint, endPoint, color,1.0f);
	gLine->Draw(startPoint, endPoint, color, 1.0f);
	glLineWidth(1.0f);
}

void DrawLib::drawRay(const Ray & r, const Color & color)
{
	//drawLine(r.pos, r.pos + (r.dir*r.maxt), color);
	//drawLine(r.pos, r.pos + (r.dir*r.mint), color);

	DrawLib::setMatricesFromStack();
	gLine->Draw(r.pos, r.pos + (r.dir*r.maxt), color);
	gLine->Draw(r.pos, r.pos + (r.dir*r.mint), color);
}

//
// drawQuad()
//
void DrawLib::drawQuad(const Point & a, const Point & b, const Point & c, const Point & d)
{
	DrawLib::setMatricesFromStack();
	gQuad->Draw(a, b, c, d, Color(1.0,1.0,1.0)); // this is the vertex color. 
}

void DrawLib::DrawLineStrip( Point *points, int n) {
	DrawLib::setMatricesFromStack();
	GLLineStrip *ls = new GLLineStrip("lineStrip", points, n, gShaders.getActiveID());
	if (ls == NULL) {
		cerr << "ERROR: DrawLineStrip cannot allocate memory!\n";
		exit (-1);
	}
	ls->Draw();
	delete ls;
}
void DrawLib::drawQuad(const Point & a, const Point & b, const Point & c, const Point & d, const Color &color)
{
	DrawLib::setMatricesFromStack();
	//drawQuad(a,b,c,d,color);
	gQuad->Draw(a, b, c, d, color);
}


//
// drawStar()
//
void DrawLib::drawStar(const Point & pos, const Vector & dir, float radius, const Color& color)
{
	
	//Vector forward = radius * normalize(dir);
	//Vector side;
	//side = cross(forward, Vector(0.0f, 1.0f, 0.0f));

	//float theta = M_PI/4;

	DrawLib::glColor(color);

	//// draw star
	//drawQuad(pos-side, pos-forward, pos+side, pos+forward);
	//drawQuad(pos - rotateInXZPlane(side,theta), pos - rotateInXZPlane(forward,theta), pos + rotateInXZPlane(side,theta), pos + rotateInXZPlane(forward,theta));
	
	gMS.Push(); {
		gMS.Translate(glm::vec3(pos[0], pos[1], pos[2]));
		gMS.Scale(glm::vec3(radius, radius, radius));
		DrawLib::setMatricesFromStack();
		gStar->Draw();
	}
	gMS.Pop();
	
}


void DrawLib::drawFlag(const Point & loc, const Color& color, float scale)
{

	checkError();
	gMS.Push();
	{
		gMS.Translate(glm::vec3(loc.x, loc.y, loc.z));
		DrawLib::setColour(color.r,color.g,color.b);
		gMS.Scale(glm::vec3(scale, scale, scale));
		
		gMS.Push(); {
			gMS.Scale(glm::vec3(0.1f, 1.f, 0.1f));
			// Canonical cylinder low base centered at origin, height 1, radius 1, axis y
			DrawLib::setMatricesFromStack();
			gCylinder->Draw();
		}
		gMS.Pop();
	
		setColour(0.,0.5, 1.);
		
		Point a = Point(0.f, 0.5f, 0.f);
		Point b = a + Point(0.5f, 0.f, 0.f);
		Point c = b + Point(0.f, 0.5f, 0.f);
		Point d = a + Point(0.f, 0.5f, 0.f);
		
		drawQuad(a,b,c,d,Color(0.,1.0,0.));
	}
	gMS.Pop();
}

void DrawLib::drawCircle(const Point & loc, const Color& color, float scale, int points)
{
	gMS.Push(); {
		gMS.Scale(glm::vec3(scale, scale, 1.0f));
		gMS.Translate(glm::vec3(loc[0], loc[1], loc[2]));

		DrawLib::setMatricesFromStack();
		gCircle->Draw();
	}
	gMS.Pop();
}



//
// drawAgentDisc() - use only after the agent display lists are built.
//
void DrawLib::drawAgentDisc(const Point & pos, const Vector & dir, float radius, const Color& color)
{
	gMS.Push();
	{
		float rad = atan2(dir.z, dir.x)*(-M_180_OVER_PI);
		DrawLib::glColor(color);
		gMS.Translate(glm::vec3(pos[0],pos[1],pos[2]));
		gMS.Rotate(rad,glm::vec3(0.0f,1.0f,0.0f));
		gMS.Scale(glm::vec3(radius, radius*4.0f, radius));

		GLdrawAgent();
	}
	gMS.Pop();
}

void DrawLib::drawAgentDisc(const Point & pos, const Vector & dir, const Vector & up, float radius, const Color& color)
{
	gMS.Push();
	{
		float yaxisrad = atan2(dir.z, dir.x)*(-M_180_OVER_PI);
		float xaxisrad = atan2(dir.y, dir.z)*(-M_180_OVER_PI);
		float zaxisrad = atan2(dir.y, dir.x)*(-M_180_OVER_PI);
		DrawLib::glColor(color);

		gMS.Translate(glm::vec3(pos[0], pos[1], pos[2]));
	
		gMS.Rotate(yaxisrad,glm::vec3(0.0f,1.0f,0.0f));
		gMS.Rotate(xaxisrad,glm::vec3(1.0f,0.0f,0.0f));
		gMS.Rotate(zaxisrad,glm::vec3(0.0f,0.0f,1.0f));
		gMS.Scale(glm::vec3(radius, radius*4.0f, radius));

		GLdrawAgent();
	}
	glPopMatrix();
}
//
// drawAgentDisc() - use only after the agent display lists are built.
//
void DrawLib::drawAgentDisc(const Point & pos, float radius, const Color& color)
{
	gMS.Push();
	{
		DrawLib::glColor(color);
		gMS.Translate(glm::vec3(pos[0],pos[1],pos[2]));
		gMS.Scale(glm::vec3(radius, radius*4.0f, radius));

		GLdrawAgentDot();
	}
	gMS.Pop();
}

//
void DrawLib::drawBoxWireFrame(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax) {
	drawBox(xmin, xmax, ymin, ymax, zmin, zmax, false);
}


//
// drawBox() - draws am axis aligned  box 
//
void DrawLib::drawBox(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax, bool fill)
{
	if ( fill )
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// scale and translate
	float sx = xmax - xmin;
	float sy = ymax - ymin;
	float sz = zmax - zmin;

	gMS.Push(); {
		gMS.Translate(glm::vec3(xmin, ymin, zmin)); // move lower corner to desired location
		gMS.Scale(glm::vec3(sx, sy, sz)); // scale to dimensions
		
		gMS.Scale(glm::vec3(0.5f, 0.5f, 0.5f)); // scale to dimension 1
		gMS.Translate(glm::vec3(1.0f, 1.0f, 1.0f)); // move lower corner to origin
		DrawLib::setMatricesFromStack();
		gCube->Draw();
	}
	gMS.Pop();
}


//
// drawCylinder() - draws a cylinder 
//
void DrawLib::drawCylinder(const Point & pos, float radius, float ymin, float ymax )
{
	
	gMS.Push();
	{
		gMS.Translate(glm::vec3(pos[0], pos[1], pos[2]));

		gMS.Scale(glm::vec3(radius, fabsf(ymax-ymin), radius));
		DrawLib::setMatricesFromStack();
		gCylinder->Draw();
	}
	gMS.Pop();
}

void DrawLib::drawCylinder(const Point & pos, float radius, float ymin, float ymax, Color color)
{
	DrawLib::glColor(color);
	drawCylinder(pos ,radius ,ymin ,ymax );
}

//
// drawHighlight() - draws a highlight that can be used to identify various objects for annotation
//
void DrawLib::drawHighlight(const Point & pos, const Vector & dir, float radius, const Color& color)
{
	drawStar(pos+Vector(0.f,0.249f,0.f), dir, radius*1.75f, color);
}


void DrawLib::GLdrawAgent(bool withTriangle)
{
	static const float h = .5;

	enableLights();
	setColour(0.0, 0.5, 0.0f);
	
	gMS.Push(); {	
		gMS.Scale(glm::vec3(1.0f, 0.5f, 1.0f));
		DrawLib::setMatricesFromStack();
		gCylinder->Draw();
	}
	gMS.Pop();

	gMS.Push(); {
		gMS.Translate(glm::vec3(0.f, h, 0.f));
		DrawLib::setMatricesFromStack();
		setColour(0.0, 0.5, 0.0f);
		gDisk->Draw();
	}
	gMS.Pop();

	if (true) {
		// draw arrow
		//glColor3f(0.2f, 0.0f, 1.0f);
		setColour(1.0, 0.0, 0.0f);

		gMS.Push(); {
			gMS.Translate(glm::vec3(0.f, h + 0.001f, 0.f));
			// Triangle with (0,0,-1), (0,0,1), (1,0,0)
			gMS.Scale(glm::vec3(0.5f, 0.5f, 0.5f));
			DrawLib::setMatricesFromStack();
			gTriangle->Draw();

		}
		gMS.Pop();

	}
	
	
	return;
}

//
// buildAgentDisplayList
//
void DrawLib::GLdrawAgentDot(void)
{
	GLdrawAgent(false);
}



void DrawLib::glColor(const Color & color){
	setColour(color.r, color.g, color.b, 1.0f);
}

void DrawLib::setGLViewMatrix(float px, float py, float pz, float rx, float ry, float rz, float ux, float uy, float uz) {
	gMS.SetViewMatrix(glm::vec3(px,py,pz), glm::vec3(rx,ry,rz), glm::vec3(ux,uy,uz) );

}

glm::mat4 DrawLib::getModelViewMat(void) {
	return gMS.View();
}

glm::mat4 DrawLib::getProjectionMat(void) {
	return gProjMat;
}


GLGrid *DrawLib::MakeGLGrid(std::string name, float xOrigin, float zOrigin, int xGridSize, int zGridSize,
	int xNumCells, int zNumCells) {


	 GLGrid *g = new GLGrid(name, xOrigin, zOrigin,xGridSize, zGridSize,
		xNumCells, zNumCells, gShaders.getActiveID());
	 return g;
}



void DrawLib::glTranslate(float x, float y, float z) {
	gMS.Translate(glm::vec3(x, y, z));
}
void DrawLib::glScale(float sx, float sy, float sz) {
	gMS.Scale(glm::vec3(sx, sy, sz));

}
void DrawLib::glRotate(float rad, float x, float y, float z){
	gMS.Rotate(rad, glm::vec3(x, y, z));
}
void DrawLib::glPushMatrix(void) { gMS.Push() ; }
void DrawLib::glPopMatrix(void) { gMS.Pop() ; }
void DrawLib::glLoadIdentity(void) { gMS.LoadIdentity(); }

#endif // ifdef ENABLE_GUI

