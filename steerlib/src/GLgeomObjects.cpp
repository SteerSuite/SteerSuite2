///////////////////////////////////////////////////////////////////////////////
// 
// testing vertex array (glDrawElements, glDrawArrays)
//
//  Original AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
//  Changes and additions: Petros Faloutsos 2013
// CREATED: 2005-10-04
// UPDATED: 2012-07-11
///////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
#include <windows.h>
#endif

//#include "GL/glew.h"
#include "opengl.h"
#include "Geometry.h"
#include "Color.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>

#include "glm/glm/glm.hpp"

#include "program.h"

#include "GLgeomObjects.h"

using std::stringstream;
using std::cout;
using std::endl;
using std::ends;

using namespace Util;

using namespace glm ;

#ifndef M_PI
#define M_PI 3.14159
#endif




//GLuint porcupineTexture ;


// cube ///////////////////////////////////////////////////////////////////////
//    v6----- v5
//   /|      /|
//  v1------v0|
//  | |     | |
//  | |v7---|-|v4
//  |/      |/
//  v2------v3

// vertex coords array for glDrawArrays() =====================================
// A cube has 6 sides and each side has 2 triangles, therefore, a cube consists
// of 36 vertices (6 sides * 2 tris * 3 vertices = 36 vertices). And, each
// vertex is 3 components (x,y,z) of floats, therefore, the size of vertex
// array is 108 floats (36 * 3 = 108).
GLfloat vertices1[] = { 1, 1, 1,  -1, 1, 1,  -1,-1, 1,      // v0-v1-v2 (front)
                       -1,-1, 1,   1,-1, 1,   1, 1, 1,      // v2-v3-v0

                        1, 1, 1,   1,-1, 1,   1,-1,-1,      // v0-v3-v4 (right)
                        1,-1,-1,   1, 1,-1,   1, 1, 1,      // v4-v5-v0

                        1, 1, 1,   1, 1,-1,  -1, 1,-1,      // v0-v5-v6 (top)
                       -1, 1,-1,  -1, 1, 1,   1, 1, 1,      // v6-v1-v0

                       -1, 1, 1,  -1, 1,-1,  -1,-1,-1,      // v1-v6-v7 (left)
                       -1,-1,-1,  -1,-1, 1,  -1, 1, 1,      // v7-v2-v1

                       -1,-1,-1,   1,-1,-1,   1,-1, 1,      // v7-v4-v3 (bottom)
                        1,-1, 1,  -1,-1, 1,  -1,-1,-1,      // v3-v2-v7

                        1,-1,-1,  -1,-1,-1,  -1, 1,-1,      // v4-v7-v6 (back)
                       -1, 1,-1,   1, 1,-1,   1,-1,-1 };    // v6-v5-v4

// normal array
GLfloat normals1[]  = { 0, 0, 1,   0, 0, 1,   0, 0, 1,      // v0-v1-v2 (front)
                        0, 0, 1,   0, 0, 1,   0, 0, 1,      // v2-v3-v0

                        1, 0, 0,   1, 0, 0,   1, 0, 0,      // v0-v3-v4 (right)
                        1, 0, 0,   1, 0, 0,   1, 0, 0,      // v4-v5-v0

                        0, 1, 0,   0, 1, 0,   0, 1, 0,      // v0-v5-v6 (top)
                        0, 1, 0,   0, 1, 0,   0, 1, 0,      // v6-v1-v0

                       -1, 0, 0,  -1, 0, 0,  -1, 0, 0,      // v1-v6-v7 (left)
                       -1, 0, 0,  -1, 0, 0,  -1, 0, 0,      // v7-v2-v1

                        0,-1, 0,   0,-1, 0,   0,-1, 0,      // v7-v4-v3 (bottom)
                        0,-1, 0,   0,-1, 0,   0,-1, 0,      // v3-v2-v7

                        0, 0,-1,   0, 0,-1,   0, 0,-1,      // v4-v7-v6 (back)
                        0, 0,-1,   0, 0,-1,   0, 0,-1 };    // v6-v5-v4

// color array
GLfloat colors1[]   = { 1, 0, 0,   1, 0, 0,   1, 0, 0,      // v0-v1-v2 (front)
                        1, 0, 0,   1, 0, 0,   1, 0, 0,      // v2-v3-v0

                        0, 0, 0,   0, 0, 0,   0, 0, 0,      // v0-v3-v4 (right)
                        0, 0, 0,   0, 0, 0,   0, 0, 0,      // v4-v5-v0

                        0, 1, 0,   0, 1, 0,   0, 1, 0,      // v0-v5-v6 (top)
                        0, 1, 0,   0, 1, 0,   0, 1, 0,      // v6-v1-v0

                        1, 1, 0,   1, 1, 0,   1, 1, 0,      // v1-v6-v7 (left)
                        1, 1, 0,   1, 1, 0,   1, 1, 0,      // v7-v2-v1

                        0, 1, 1,   0, 1, 1,   0, 1, 1,      // v7-v4-v3 (bottom)
                        0, 1, 1,   0, 1, 1,   0, 1, 1,      // v3-v2-v7

                        0, 0, 1,   0, 0, 1,   0, 0, 1,      // v4-v7-v6 (back)
                        0, 0, 1,   0, 0, 1,   0, 0, 1 };    // v6-v5-v4

GLfloat cubeTexCoord[] = { 1,1,  0,1, 0,0,      // v0-v1-v2 (front)
    0,0,   1,0,   1, 1,      // v2-v3-v0
    
    0, 1,   0,0,  1,0,      // v0-v3-v4 (right)
    1,0,   1, 1,   0,1,      // v4-v5-v0
    
    1, 0,   1, 1,  0, 1,      // v0-v5-v6 (top)
    0, 1,   0, 0,  1, 0,      // v6-v1-v0
    
    1, 1,   0, 1,  0,0,     // v1-v6-v7 (left)
    0,0,    1,0,   1, 1,      // v7-v2-v1
    
    0,1,   0,0,   1,0,      // v7-v4-v3 (bottom)
    1,0,   1,1,    0,1,  // v3-v2-v7
    
    0,0, 1,0, 1,1,     // v4-v7-v6 (back)
    1,1, 0,1, 0,0 };    // v6-v5-v4

// color array
GLfloat colors2[]   = { 1, 1, 1,   1, 1, 0,   1, 0, 0,      // v0-v1-v2 (front)
    1, 0, 0,   1, 0, 1,   1, 1, 1,      // v2-v3-v0
    
    1, 1, 1,   1, 0, 1,   0, 0, 1,      // v0-v3-v4 (right)
    0, 0, 1,   0, 1, 1,   1, 1, 1,      // v4-v5-v0
    
    1, 1, 1,   0, 1, 1,   0, 1, 0,      // v0-v5-v6 (top)
    0, 1, 0,   1, 1, 0,   1, 1, 1,      // v6-v1-v0
    
    1, 1, 0,   0, 1, 0,   0, 0, 0,      // v1-v6-v7 (left)
    0, 0, 0,   1, 0, 0,   1, 1, 0,      // v7-v2-v1
    
    0, 0, 0,   0, 0, 1,   1, 0, 1,      // v7-v4-v3 (bottom)
    1, 0, 1,   1, 0, 0,   0, 0, 0,      // v3-v2-v7
    
    0, 0, 1,   0, 0, 0,   0, 1, 0,      // v4-v7-v6 (back)
    0, 1, 0,   0, 1, 1,   0, 0, 1 };    // v6-v5-v4


using namespace std;



GLObject::~GLObject() {
	glDeleteBuffers(1, &_vbo);
	glDeleteBuffers(1, &_vao);
	
};
void GLObject::binVAO(void) {
	glBindVertexArray(_vao);
};

// Retrieves the locations of the following attributes:
// "position", "colour", "normal", "VertexTexCoord"
void GLObject::getAttributeLocations(GLint prog,
                                   GLint &positionAttribute,
                                   GLint &normalAttribute,
                                   GLint &colorAttribute,
                                   GLint &texCoordAttribute)
{
    // Get attribute locations
   
	positionAttribute = glGetAttribLocation(prog, "VertexPosition");
	if (positionAttribute < 0) {
		cerr << "Shader did not contain the 'VertexPosition' attribute." << endl;
	}
    
    colorAttribute = glGetAttribLocation(prog, "VertexColour");
	if (colorAttribute < 0) {
		cerr << "Object::getAttributeLocations: Shader did not contain the 'VertexColour' attribute." << endl;
	}
    
    normalAttribute = glGetAttribLocation(prog, "VertexNormal");
	if ( normalAttribute < 0) {
		cerr << "Shader did not contain the 'VertexNormal' attribute." << endl;
	}
    
    texCoordAttribute = glGetAttribLocation(prog, "VertexTexCoord");
	if (texCoordAttribute < 0) {
		cerr << "Shader did not contain the 'VertexTexCoord' attribute." << endl;
	}
    
}

void GLObject::SetBuffers(GLuint prog)
{
  
    GLint positionAttribute, colourAttribute, normalAttribute, texCoordAttribute;
    
    glGenVertexArrays(1, &_vao);
    checkError() ;
    glBindVertexArray(_vao);
    checkError() ;
    
    
	checkError() ;
    glGenBuffers(1, &_vbo);
	checkError() ;
    
    // bind the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER,_vbo);
    //populate the vertex buffer
    SetData() ;
    checkError() ;
    
    getAttributeLocations(prog, positionAttribute, normalAttribute, colourAttribute, texCoordAttribute) ;
	checkError() ;
	glEnableVertexAttribArray((GLuint)positionAttribute);
    glEnableVertexAttribArray((GLuint)colourAttribute);
    glEnableVertexAttribArray((GLuint)normalAttribute) ;
    glEnableVertexAttribArray((GLuint)texCoordAttribute) ;

	checkError() ;

   // glBindBuffer(GL_ARRAY_BUFFER, _vbo) ; // bind the vdo that contains the attributes
    glVertexAttribPointer((GLuint) positionAttribute, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const GLvoid *)0);
    glVertexAttribPointer((GLuint) colourAttribute, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const GLvoid *) sizeof(vec4));
    glVertexAttribPointer((GLuint) normalAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const GLvoid *) (2*sizeof(vec4)));
    glVertexAttribPointer((GLuint) texCoordAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const GLvoid *) (2*sizeof(vec4) + sizeof(vec3)));
    
	checkError();

    glBindVertexArray(0) ; // unbind the vertex array
    
}



bool flip(const VertexData &vd1,const VertexData &vd2, const VertexData &vd3){
    
    // compute average normal
    vec3 an = 1.0f/3.0f*(vd1.normal + vd2.normal + vd3.normal) ;
    // compute from triangle
    vec3 va = vd2.normal - vd1.normal ;
    vec3 vb = vd3.normal - vd1.normal ;
    vec3 tn = glm::cross(vb,va) ;
    if( dot(an,tn) < 0) return true ;
    
    return false ;
}



GLGrid::GLGrid(std::string name, float xOrigin, float zOrigin, int xGridSize, int zGridSize, 
	int xNumCells, int zNumCells, GLuint prog) {

	_origin = glm::vec2(xOrigin, zOrigin);
	_size = glm::ivec2(xGridSize, zGridSize);
	_numCells = glm::ivec2(xNumCells, zNumCells);
	_n = 2*(xNumCells + 1)+2*(zNumCells + 1);

	SetBuffers(prog);
	return;
}

void GLGrid::Draw(void) {

	glBindVertexArray(_vao);
	glDrawArrays(GL_LINES, 0, _n);
	checkError();
	glBindVertexArray(0);

}

void GLGrid::SetData(void) {
	if (_n < 1) return;
	VertexData *vertexDataArray = new VertexData[_n];
	if (vertexDataArray == NULL) {
		cerr << "ERROR: GLGrid data cannot allocate memory\n";
		return;
	}

	glm::vec4 c = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	float d = _size[0] / _numCells[0];
	int ncount = 0;
	for (int i = 0; i <= _numCells[0]; i++) {
		vertexDataArray[ncount].position = glm::vec4(_origin[0] + i*d, 0.f, _origin[1], 1.0f);
		vertexDataArray[ncount].texCoord = glm::vec2((float)i / (float)_numCells[0], 0.f);
		ncount++;

		vertexDataArray[ncount].position = glm::vec4(_origin[0] + i*d, 0.f, _origin[1]+_size[1], 1.0f);
		vertexDataArray[ncount].texCoord = glm::vec2((float)i / (float)_numCells[0], 1.f);
		ncount++;
	}

	d = (float) _size[1] / _numCells[1];
	for (int i = 0; i <= _numCells[1]; i++) {
		vertexDataArray[ncount].position = glm::vec4(_origin[0], 0.f, _origin[1] + i*d, 1.0f);
		vertexDataArray[ncount].texCoord = glm::vec2((float)i / (float)_numCells[1], 0.f);
		ncount++;

		vertexDataArray[ncount].position = glm::vec4(_origin[0]+_size[0],0.f, _origin[1] + i*d, 1.0f);
		vertexDataArray[ncount].texCoord = glm::vec2((float)i / (float)_numCells[1], 1.f);
		ncount++;
	}

	glm::vec3 y = glm::vec3(0.f, 1.f, 0.f);
	for (int i = 0; i < _n; i++) {
		vertexDataArray[i].normal = y;
		vertexDataArray[i].colour= c;
	}
	glBufferData(GL_ARRAY_BUFFER, _n * sizeof(VertexData), vertexDataArray, GL_STATIC_DRAW);
	checkError();
	delete[] vertexDataArray;

}



GLLineStrip::GLLineStrip(std::string name, Point *points, int n, GLuint prog){
	_n = n;
	_p = points;
	SetBuffers(prog);
	return;
}

void GLLineStrip::SetData(void) {
	if (_n < 1) return;
	VertexData *vertexDataArray = new VertexData[_n];
	if (vertexDataArray == NULL) {
		cerr << "ERROR: GLCircle data cannot allocate memory\n";
		return;
	}

	for (int i = 0; i < _n; i++) {
		vertexDataArray[i].position = glm::vec4(_p[i].x, _p[i].y, _p[i].z, 1.0f);
		vertexDataArray[i].normal = glm::vec3(0.f, 1.0f, 0.f);
		vertexDataArray[i].colour = glm::vec4(1.f, 1.0f, 1.f,1.0f);
		vertexDataArray[i].texCoord = glm::vec2((float) i/ (float) _n, 0.f);
	}
	glBufferData(GL_ARRAY_BUFFER, _n * sizeof(VertexData), vertexDataArray, GL_STATIC_DRAW);
	checkError();

	delete [] vertexDataArray;
}

void GLLineStrip::Draw(void) {
	if (_n < 1) return;

	glBindVertexArray(_vao);
	glDrawArrays(GL_LINE_STRIP, 0, _n);
	checkError();
	glBindVertexArray(0);
}

GLLine::GLLine(std::string name, GLuint prog) {
	SetBuffers(prog);
	checkError();
}

void GLLine::SetData(void){

	VertexData vertexData[2] = {
		vec4(0.f, 0.f, 0.f, 1.f), vec4(0.f, 0.f, 0.f, 1.f), vec3(0.f, 0.f, 1.f), vec2(0.f, 0.f),
		vec4(0.f, 0.f, 0.f, 1.f), vec4(0.f, 0.f, 0.f, 1.f), vec3(0.f, 0.f, 1.f), vec2(1.f, 0.f) };

	glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(VertexData), vertexData, GL_STATIC_DRAW);
	checkError();
}

void GLLine::Draw(void) {

	checkError();
	glBindVertexArray(_vao);
	glDrawArrays(GL_LINES, 0, 2);
	checkError();
	glBindVertexArray(0);
	checkError();
}

void GLLine::Draw(const Point & p1, const Point & p2, const Color &c, float alpha) {
	checkError();
	glm::vec4 color(c.r, c.g, c.b, alpha);
	this->Draw(p1, p2, color);
	checkError();
}

void GLLine::Draw(const Point & p1, const Point & p2, const Color & color) {
	checkError();
	glm::vec4 c(color.r, color.g, color.b, 1.0f);
	this->Draw(p1, p2, c);
	checkError();
}


void GLLine::Draw(const Point & p1, const Point & p2, glm::vec4 color) {

	checkError();
	VertexData vertexData[2] = {
		vec4(0.f, 0.f, 0.f, 1.f), vec4(0.f, 0.f, 0.f, 1.f), vec3(0.f, 0.f, 1.f), vec2(0.f, 0.f),
		vec4(0.f, 0.f, 0.f, 1.f), vec4(0.f, 0.f, 0.f, 1.f), vec3(0.f, 0.f, 1.f), vec2(1.f, 0.f) };

	vertexData[0].position = glm::vec4(p1[0], p1[1], p1[2], 1.0f);
	vertexData[1].position = glm::vec4(p2[0], p2[1], p2[2], 1.0f);
	vertexData[0].colour = color;
	vertexData[1].colour = color;

	glBindVertexArray(this->_vao);
	glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 2 * sizeof(VertexData), vertexData);
	checkError();
	this->Draw();
	checkError();
	glBindVertexArray(0) ;
	checkError();

}

void GLLine::Draw(const Point & p1, const Point & p2) {
	checkError();
	Color c(0.f, 0.f, 0.f);
	Draw(p1, p2, c);
	checkError();
	

}


Sphere::Sphere(std::string name, int n, GLuint prog)
{
    if( n < 1) return ;
    _n = n ;
    SetBuffers(prog) ;
    return ;
}

void Sphere::Draw() {
    

    glFrontFace(GL_CW) ;
    glEnable(GL_CULL_FACE) ;
    //glDisable(GL_CULL_FACE) ;
    
    glBindVertexArray(_vao);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArrays(GL_TRIANGLES, 0,_n*_n*6 );
    checkError() ;
    glBindVertexArray(0) ;
    
}

void GetVertexSphere(VertexData &vd, float uu, float vv)
{
    //float v = 2*M_PI*vv - M_PI;
    //float u = 2*M_PI*uu  ;
    
    // epsilon
    //float e = 0.05; // in parametric space
    
    //float v = M_PI*(vv*(1-e) + e) ;
    float v = M_PI*vv ;
    float u = 2*M_PI* uu ;
    vd.position[0] = cos(u)*sin(v);
    vd.position[1] = sin(u)*sin(v) ;
    vd.position[2] = cos(v) ;
    vd.position[3] = 1.0 ;
    
    vd.normal[0] = vd.position[0] ;
    vd.normal[1] = vd.position[1] ;
    vd.normal[2] = vd.position[2] ;
    
    vd.colour = vec4(1.0,0.0,0.0,1.0) ;
    
    vd.texCoord[0] = uu ;
    vd.texCoord[1] = vv ;
    //std::cerr << vv << ", " << uu << std::endl ;
    
    return;
}

GLCircle::GLCircle(std::string name, int n, GLuint prog) {
	if (n < 1) return;
	_n = n;
	SetBuffers(prog);
	return;
}

void GLCircle::SetData() {
	if (_n < 1) return;
	VertexData *vertexDataArray = new VertexData[_n];
	if (vertexDataArray == NULL) {
		cerr << "ERROR: GLCircle data cannot allocate memory\n";
		return;
	}

	for (int i = 0; i < _n; i++) {
		float u = i*2 * M_PI / (float)_n;
		vertexDataArray[i].normal = glm::vec3(0.f, 0.f, 1.0f); // positive z
		vertexDataArray[i].position = glm::vec4(0.5*cos(u), 0.5*sin(u), 0.f,1.0f);
		vertexDataArray[i].colour = glm::vec4(0.f, 0.f, 0.f, 1.f);
		vertexDataArray[i].texCoord[0] = 0.5f + vertexDataArray[i].position[0];
		vertexDataArray[i].texCoord[1] = 0.5f + vertexDataArray[i].position[1];

	}
	glBufferData(GL_ARRAY_BUFFER, _n * sizeof(VertexData), vertexDataArray, GL_STATIC_DRAW);
	checkError();

	delete[] vertexDataArray;
}

void GLCircle::Draw(void) {

	glFrontFace(GL_CCW);
	glDisable(GL_CULL_FACE);
	glBindVertexArray(_vao);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	checkError();
	glDrawArrays(GL_LINE_LOOP, 0, _n);
	checkError();
	glBindVertexArray(0);
	checkError();
}

///@ Disk of radius one centered at the origin in the xz plane
GLDisk::GLDisk(std::string name, int n, GLuint prog) {
	if (n < 1) return;
	_n = n +2 ; // add the center point of the fan and the last vertex which is the first on the perimeter to close it
	SetBuffers(prog);
	return;
}

void GLDisk::SetData() {
	if (_n < 1) return;
	VertexData *vertexDataArray = new VertexData[_n];
	if (vertexDataArray == NULL) {
		cerr << "ERROR: GLDisk data cannot allocate memory\n";
		return;
	}

	// first vertex is the center of the fan
	vertexDataArray[0].position = glm::vec4(0.f, 0.f, 0.f,1.0f);
	vertexDataArray[0].normal = glm::vec3(0.f, 1.f, 0.0f);
	vertexDataArray[0].colour = glm::vec4(0.f, 0.f, 0.f, 1.f);
	vertexDataArray[0].texCoord = glm::vec2(0.f, 0.f);

	int n = _n - 2; // just the distinct perimeter vertices
	for (int i = 0; i < n; i++) {
		float u = i * 2 * M_PI / (float) n;
		vertexDataArray[i+1].normal = glm::vec3(0.f, 1.f, 0.f); // positive z
		vertexDataArray[i+1].position = glm::vec4(0.5*cos(u), 0.f, 0.5*sin(u), 1.0f);
		vertexDataArray[i+1].colour = glm::vec4(0.f, 0.f, 0.f, 1.f);
		vertexDataArray[i+1].texCoord[0] = 0.5f + vertexDataArray[i+1].position[0];
		vertexDataArray[i+1].texCoord[1] = 0.5f + vertexDataArray[i+1].position[1];
	}
	// add the the second vertex again to close the fan
	vertexDataArray[_n - 1] = vertexDataArray[1];

	glBufferData(GL_ARRAY_BUFFER, _n * sizeof(VertexData), vertexDataArray, GL_STATIC_DRAW);
	checkError();

	delete[] vertexDataArray;
}

void GLDisk::Draw(void) {


	glFrontFace(GL_CW);
	glDisable(GL_CULL_FACE);
	glBindVertexArray(_vao);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	checkError();
	glDrawArrays(GL_TRIANGLE_FAN, 0, _n);
	checkError();
	glBindVertexArray(0);
	checkError();
}

// @ A star shape of radius 1 with n points. N must be multiple of 3
GLStar::GLStar(std::string name, int n, GLuint prog) {
	if (n < 1) return;
	_n = n;
	SetBuffers(prog);
	return;
}

void GLStar::SetData() {
	if (_n < 1) return;
	VertexData *vertexDataArray = new VertexData[_n];
	if (vertexDataArray == NULL) {
		cerr << "ERROR: GLCircle data cannot allocate memory\n";
		return;
	}

	for (int i = 0; i < _n; i++) {
		float u = i * 2 * M_PI / (float)_n;
		vertexDataArray[i].normal = glm::vec3(0.f, 0.f, 1.0f); // positive z
		if ( i % 3 == 0) {
			vertexDataArray[i].position = glm::vec4(cos(u), sin(u), 0.f, 1.0f);
		}
		else {
			vertexDataArray[i].position = glm::vec4(0.7*cos(u), 0.7*sin(u), 0.f, 1.0f);
		}
		vertexDataArray[i].position = glm::vec4(0.5f*cos(u), 0.5f*sin(u), 0.f, 1.0f);
		vertexDataArray[i].colour = glm::vec4(0.f, 0.f, 0.f, 1.f);
		vertexDataArray[i].texCoord[0] = 0.5f + vertexDataArray[i].position[0];
		vertexDataArray[i].texCoord[1] = 0.5f + vertexDataArray[i].position[1];

	}
	glBufferData(GL_ARRAY_BUFFER, _n * sizeof(VertexData), vertexDataArray, GL_STATIC_DRAW);
	checkError();

	delete[] vertexDataArray;
}

void GLStar::Draw(void) {


	glFrontFace(GL_CCW);
	glDisable(GL_CULL_FACE);
	glBindVertexArray(_vao);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	checkError();
	glDrawArrays(GL_LINE_LOOP, 0, _n);
	checkError();
	glBindVertexArray(0);
	checkError();
}

// @ An triangle in XZ plane with coordinates (1,0,0) (-1,0,0) (0,0,1) centered at the origin with Apex at the z Axis.
GLTriangle::GLTriangle(std::string name, GLuint prog)
{
    SetBuffers(prog) ;
    return ;
}

void GLTriangle::SetData(void){

	VertexData vertexData[3] = {
		{ vec4(1.f, 0.f, -0.f, 1.f), vec4(1.0f, 0.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f) },
		{ vec4(-1.f, 0.f, 0.f, 1.f), vec4(0.0f, 1.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 0.0f) },
		{ vec4(0.f, 0.f, 1.0f, 1.f), vec4(0.0f, 0.0f, 1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f), vec2(0.5f, 1.0f) },
	};

	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(VertexData), vertexData, GL_STATIC_DRAW);
	checkError();
}

void GLTriangle::Draw(void) {

    glFrontFace(GL_CCW) ;
    glDisable(GL_CULL_FACE) ;
    glBindVertexArray(_vao);
    checkError() ;
    glDrawArrays(GL_TRIANGLES, 0, 3);
    checkError() ;
    glBindVertexArray(0) ;
    checkError() ;
}

Square::Square(std::string name, GLuint prog)
{
	SetBuffers(prog);
	return;
}

void Square::SetData(void){

	VertexData vertexData[4] = {
		{ vec4(-0.5f, -0.5f, 0.0f, 1.0f), vec4(1.0f, 0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f) },
		{ vec4(0.5f, -0.5f, 0.0f, 1.0f), vec4(0.0f, 1.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f) },
		{ vec4(0.5f, 0.5f, 0.0f, 1.0f), vec4(0.0f, 0.0f, 1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f) },
		{ vec4(-0.5f, 0.5f, 0.0f, 1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f) }
	};

	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(VertexData), vertexData, GL_STATIC_DRAW);
	checkError();
}

void Square::Draw(void) {


	glFrontFace(GL_CCW);
	glDisable(GL_CULL_FACE);
	glBindVertexArray(_vao);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	checkError();
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	checkError();
	glBindVertexArray(0);
	checkError();
}





Quad::Quad(std::string name, GLuint prog)
{
	SetBuffers(prog);
	return;
}

void Quad::SetData(void){

	VertexData vertexData[4] = {
		{ vec4(-0.5f, -0.5f, 0.0f, 1.0f), vec4(1.0f, 0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f) },
		{ vec4(0.5f, -0.5f, 0.0f, 1.0f), vec4(0.0f, 1.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f) },
		{ vec4(0.5f, 0.5f, 0.0f, 1.0f), vec4(0.0f, 0.0f, 1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f) },
		{ vec4(-0.5f, 0.5f, 0.0f, 1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f) }
	};

	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(VertexData), vertexData, GL_STATIC_DRAW);
	checkError();
}

void Quad::Draw(void) {


	glFrontFace(GL_CCW);
	glDisable(GL_CULL_FACE);
	glBindVertexArray(_vao);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	checkError();
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	checkError();
	glBindVertexArray(0);
}

void Quad::Draw(const Point & a, const Point & b, const Point & c, const Point & d, const Color &color)
{
	glm::vec4 col = glm::vec4(color.r, color.g, color.b, 1.0f);
	this->Draw(a, b, c, d, col);

}

void Quad::Draw(const Point & a, const Point & b, const Point & c, const Point & d, glm::vec4 color)
{

	VertexData vertexData[4] = {
		{ vec4(-0.5f, -0.5f, 0.0f, 1.0f), vec4(1.0f, 0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f) },
		{ vec4(0.5f, -0.5f, 0.0f, 1.0f), vec4(0.0f, 1.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f) },
		{ vec4(0.5f, 0.5f, 0.0f, 1.0f), vec4(0.0f, 0.0f, 1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f) },
		{ vec4(-0.5f, 0.5f, 0.0f, 1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f) }
	};

	vertexData[0].position = glm::vec4(a[0], a[1], a[2], 1.0f);
	vertexData[1].position = glm::vec4(b[0], b[1], b[2], 1.0f);
	vertexData[2].position = glm::vec4(c[0], c[1], c[2], 1.0f);
	vertexData[3].position = glm::vec4(d[0], d[1], d[2], 1.0f);

	vertexData[0].colour = color;
	vertexData[1].colour = color;
	vertexData[2].colour = color;
	vertexData[3].colour = color;

	// compute normal
	glm::vec3 p1 = glm::vec3(vertexData[0].position);
	glm::vec3 p2 = glm::vec3(vertexData[1].position);
	glm::vec3 p3 = glm::vec3(vertexData[2].position);

	glm::vec3 n = glm::normalize(glm::cross(p2-p1, p3-p1));
	vertexData[0].normal = n;
	vertexData[1].normal = n;
	vertexData[2].normal = n;
	vertexData[3].normal = n;

	glBindVertexArray( this->_vao);
	glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * sizeof(VertexData), vertexData);
	this->Draw();
}


Cube::Cube(std::string name, GLuint prog)
{
    SetBuffers(prog) ;
    return ;
}

void Cube::SetData(void)
{
    
    checkError() ;
    
    // put all elements in one array

    
    VertexData vertexDataArray[36] ;
    
    int count = 0, texCount = 0 ;
    for( int i = 0 ; i < 36; i++){
        vertexDataArray[i].position[0] = vertices1[count] ;
        vertexDataArray[i].position[1] = vertices1[count+1] ;
        vertexDataArray[i].position[2] = vertices1[count+2] ;
        vertexDataArray[i].position[3] = 1.0 ;
        
        
        vertexDataArray[i].normal[0] = normals1[count] ;
        vertexDataArray[i].normal[1] = normals1[count + 1] ;
        vertexDataArray[i].normal[2] = normals1[count + 2] ;
        
        vertexDataArray[i].colour[0] = colors1[count] ;
        vertexDataArray[i].colour[1] = colors1[count + 1] ;
        vertexDataArray[i].colour[2] = colors1[count + 2] ;
        vertexDataArray[i].colour[3] = 1.0 ;
        
        
        vertexDataArray[i].texCoord[0] = cubeTexCoord[texCount] ;
        vertexDataArray[i].texCoord[1] = cubeTexCoord[texCount+1] ; ;
        
        
        //cout << vertexDataArray[i].position << " --- " << vertices1[count] << ","
        //    << vertices1[count+1] << "," << vertices1[count+2] << endl ;
        //cout << vertexDataArray[i].position << "-- "
        //<< vertexDataArray[i].colour << endl ;
        count = count + 3 ;
        texCount = texCount + 2 ;
        
    }
    
    glBufferData(GL_ARRAY_BUFFER, 36*sizeof(VertexData), vertexDataArray, GL_STATIC_DRAW);
    checkError() ;
    
}


void Cube::Draw(void) {


    glFrontFace(GL_CCW) ;
    //glCullFace(GL_FRONT);
    
    //glEnable(GL_CULL_FACE) ;
    glDisable(GL_CULL_FACE) ;
    
    glBindVertexArray(_vao);


    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0) ;
    
}

// @ Cylinder of radius 1 and height one with the lower base centered at the origin and major axis  y 
Cylinder::Cylinder(std::string name, int n, GLuint prog)
{
    if( n < 1) return ;
    _n = n ;
    SetBuffers(prog) ;
    return ;
}



void GetVertexCylinder(VertexData &vd, float u, float v)
{
    
    vd.position[0] = 0.5f*cos(u*2*M_PI);
    vd.position[1] = v ;
	vd.position[2] = 0.5f*sin(u * 2 * M_PI);
	vd.position[3] = 1.0f ;
    
    vd.normal[0] = cos(u*2*M_PI) ;
	vd.normal[1] = 0.0;
    vd.normal[2] = sin(u*2*M_PI) ;
    
    vd.colour = vec4(1.0f,0.0f,0.0f,1.0f) ;
    
    vd.texCoord[0] = u ;
    vd.texCoord[1] = v ;
    
    return;
}


void Cylinder::SetData(void)
{
    
    if( _n < 1) return;
    
    VertexData *vertexDataArray = new VertexData[_n*_n*6] ;
    
    checkError() ;
    
    float du = 1.0f / (float) _n ;
    float dv = du ;
    // do it by quads made up of two triangles
    int count = 0 ;
    for( float u = 0 ; u < 1.0 ; u += du) {
        for( float v = 0 ; v < 1.0  ; v += dv) {
            //cerr << "----------------------------\n" ;
            //cerr << "(" << u << "," << v << ")" << endl ;
            //cerr << "(" << u+du << "," << v << ")" << endl ;
            //cerr << "(" << u+du << "," << v+dv << ")" << endl ;
            //cerr << "(" << u << "," << v+dv << ")" << endl ;
           
            // make them into triangles
            VertexData vd1,vd2,vd3,vd4 ;
            GetVertexCylinder(vd1, u,v) ;
            GetVertexCylinder(vd2, u+du,v) ;
            GetVertexCylinder(vd3, u+du,v+dv) ;
            GetVertexCylinder(vd4, u,v+dv) ;
            
            
            vertexDataArray[count++] = vd1 ;
            vertexDataArray[count++] = vd2 ;
            vertexDataArray[count++] = vd3 ;
            
           
            vertexDataArray[count++] = vd3 ;
            vertexDataArray[count++] = vd4 ;
            vertexDataArray[count++] = vd1 ;
            
        }
           
    }
    glBufferData(GL_ARRAY_BUFFER, _n*_n*6*sizeof(VertexData), vertexDataArray, GL_STATIC_DRAW);
    checkError() ;
    delete [] vertexDataArray ;
}



void Cylinder::Draw() {



    glFrontFace(GL_CW) ;
    //glEnable(GL_CULL_FACE) ;
    //glDisable(GL_CULL_FACE) ;
    
    glBindVertexArray(_vao);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArrays(GL_TRIANGLES, 0,_n*_n*6 );
    checkError() ;
    glBindVertexArray(0) ;
    
}

Cone::Cone(std::string name, int n, GLuint prog)
{
    if( n < 1) return ;
    _n = n ;
    SetBuffers(prog) ;
    return ;
}

void Cone::Draw() {
    

    glFrontFace(GL_CCW) ;
    //glEnable(GL_CULL_FACE) ;
    //glDisable(GL_CULL_FACE) ;
    
    glBindVertexArray(_vao);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArrays(GL_TRIANGLES, 0,_n*_n*6 );
    checkError() ;
    glBindVertexArray(0) ;
    
}


void GetVertexCone(VertexData &vd, float u, float v)
{
    float radius = 1.0f - v ;
    
    vd.position[0] = radius*cos(u*2*M_PI);
    vd.position[1] = radius*sin(u*2*M_PI) ;
    vd.position[2] = (v-0.5f) ;
    vd.position[3] = 1.0f ;
    
    glm::vec3 n(cos(u*2*M_PI), sin(u*2*M_PI), 1.0) ;
    vd.normal = glm::normalize(n) ;
    
    vd.colour = vec4(1.0,0.0,0.0,1.0) ;
    
    vd.texCoord[0] = u ;
    vd.texCoord[1] = v ;
    
    return;
}

void Cone::SetData(void){
    
    if( _n < 1) return;
    
    VertexData *vertexDataArray = new VertexData[_n*_n*6] ;
    
    checkError() ;
    
    float du = 1.0f / (float) _n ;
    float dv = du ;
    // do it by quads made up of two triangles
    int count = 0 ;
    for( float u = 0 ; u < 1.0 ; u += du) {
        for( float v = 0 ; v < 1.0  ; v += dv) {
            //cerr << "----------------------------\n" ;
            //cerr << "(" << u << "," << v << ")" << endl ;
            //cerr << "(" << u+du << "," << v << ")" << endl ;
            //cerr << "(" << u+du << "," << v+dv << ")" << endl ;
            //cerr << "(" << u << "," << v+dv << ")" << endl ;
            
            // make them into triangles
            VertexData vd1,vd2,vd3,vd4 ;
            GetVertexCone(vd1, u,v) ;
            GetVertexCone(vd2, u+du,v) ;
            GetVertexCone(vd3, u+du,v+dv) ;
            GetVertexCone(vd4, u,v+dv) ;
            
            vertexDataArray[count++] = vd1 ;
            vertexDataArray[count++] = vd2 ;
            vertexDataArray[count++] = vd3 ;
            
            vertexDataArray[count++] = vd3 ;
            vertexDataArray[count++] = vd4 ;
            vertexDataArray[count++] = vd1 ;
            
        }
    }
    
    // load the attribute data into the vbo
    glBufferData(GL_ARRAY_BUFFER, _n*_n*6*sizeof(VertexData), vertexDataArray, GL_STATIC_DRAW);
    checkError() ;
    delete [] vertexDataArray ;
}


float B3(int i, float t)
{
    float t1 ;
    switch ( i ) {
  
        case 0:
            t1 = 1-t ;
            return t1*t1*t1 ;
            break ;
        case 1:
            return 3*(1-t)*(1-t)*t ;
            break ;
        case 2:
            return 3*(1-t)*t*t ;
            break ;
        case 3:
            return t*t*t ;
            break ;
        default:
            cerr << "ERROR: B3: index must be 0,1,2,3 \n" ;
            return 0 ;
    }
}


float dB3(int i, float t)
{
    float t1 ;
    switch ( i ) {
        case 0:
            t1 = 1-t ;
            return -3*t1*t1 ;
            break ;
        case 1:
            return 3+(9*t-12)*t ;
            break ;
        case 2:
            return 3*t*(2-3*t) ;
            break ;
        case 3:
            return 3*t*t ;
            break ;
        default:
            cerr << "ERROR: dB3: index must be 0,1,2,3 \n" ;
            return 0 ;
    }
}

BezierPatch3::BezierPatch3(std::string name, int n, GLfloat p[4][4][3], GLuint prog)
{
    if( n < 1) return ;
    _n = n ;
    
    SetControlPoints(p);
    SetBuffers(prog) ;
    return ;
}

void BezierPatch3::Draw() {

    
    glFrontFace(GL_CW) ;
    //glEnable(GL_CULL_FACE) ;
    //glDisable(GL_CULL_FACE) ;
    
    glBindVertexArray(_vao);
    
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glPointSize(5) ;
    glDrawArrays(GL_TRIANGLES, 0,_n*_n*6 );
    checkError() ;
    glBindVertexArray(0) ;
    
}

void BezierPatch3::GetVertex(VertexData &vd, float u, float v)
{
    
    vec3 dPu = vec3(0.f) ;
    vec3 dPv = vec3(0.f) ;
    vec3 position = vec3(0.f) ;
    
    for( int i = 0 ; i < 4 ; i++ )
    {
        float bi = B3(i,u) ;
        float dbi = dB3(i,u) ;
        for( int j = 0 ; j < 4 ; j++ )
        {
            position = position + bi*B3(j,v)*_cpoints[i][j] ;
            dPu += dbi*B3(j,v)*_cpoints[i][j] ;
            dPv += bi*dB3(j,v)*_cpoints[i][j] ;
        }
    }
    vd.position = vec4(position, 1.0f) ;
    vd.normal = glm::normalize(glm::cross(dPv,dPu)) ;
    
    vd.colour = vec4(1.0,0.0,0.0,1.0) ;
    
    vd.texCoord[0] = u ;
    vd.texCoord[1] = v ;
    //std::cerr << vv << ", " << uu << std::endl ;
    
    return;
}

void BezierPatch3::SetControlPoints(GLfloat p[4][4][3])
{
    for( int i = 0 ; i < 4 ; i++ )
        for( int j = 0 ; j < 4; j ++ )
        {
            _cpoints[i][j][0] = p[i][j][0] ;
            _cpoints[i][j][1] = p[i][j][1] ;
            _cpoints[i][j][2] = p[i][j][2] ;
            
        }
    
}

void BezierPatch3::SetData(void){
    

    if( _n < 1) return;
    
    
    VertexData *vertexDataArray = new VertexData[_n*_n*6] ;
    
    checkError() ;
    
    float du = 1.0f / (float) _n ;
    float dv = du ;
    // do it by quads made up of two triangles
    int count = 0 ;
    for( float u = 0 ; u < 1.0 ; u += du) {
        for( float v = 0 ; v < 1.0  ; v += dv) {
            //cerr << "----------------------------\n" ;
            //cerr << "(" << u << "," << v << ")" << endl ;
            //cerr << "(" << u+du << "," << v << ")" << endl ;
            //cerr << "(" << u+du << "," << v+dv << ")" << endl ;
            //cerr << "(" << u << "," << v+dv << ")" << endl ;
            
            // make them into triangles
            VertexData vd1,vd2,vd3,vd4 ;
            GetVertex(vd1, u,v) ;
            GetVertex(vd2, u+du,v) ;
            GetVertex(vd3, u+du,v+dv) ;
            GetVertex(vd4, u,v+dv) ;
            
            vertexDataArray[count++] = vd1 ;
            vertexDataArray[count++] = vd2 ;
            vertexDataArray[count++] = vd3 ;
            
            vertexDataArray[count++] = vd3 ;
            vertexDataArray[count++] = vd4 ;
            vertexDataArray[count++] = vd1 ;
        }
    }
        
    // load the attribute data into the vbo
    glBufferData(GL_ARRAY_BUFFER, _n*_n*6*sizeof(VertexData), vertexDataArray, GL_STATIC_DRAW);
    checkError() ;
    delete [] vertexDataArray ;
}



void Sphere::SetData(void){
    
    if( _n < 1) return;
    
    VertexData *vertexDataArray = new VertexData[_n*_n*6] ;
    
    checkError() ;
    
    float du = 1.0f / (float) _n ;
    float dv = du ;
    // do it by quads made up of two triangles
    int count = 0 ;
    for( float u = 0 ; u < 1.0 ; u += du) {
        for( float v = 0 ; v < 1.0  ; v += dv) {
            //cerr << "----------------------------\n" ;
            //cerr << "(" << u << "," << v << ")" << endl ;
            //cerr << "(" << u+du << "," << v << ")" << endl ;
            //cerr << "(" << u+du << "," << v+dv << ")" << endl ;
            //cerr << "(" << u << "," << v+dv << ")" << endl ;
            
            // make them into triangles
            VertexData vd1,vd2,vd3,vd4 ;
            GetVertexSphere(vd1, u,v) ;
            GetVertexSphere(vd2, u+du,v) ;
            GetVertexSphere(vd3, u+du,v+dv) ;
            GetVertexSphere(vd4, u,v+dv) ;
            
            if( !flip(vd1,vd2,vd3) ) {
                vertexDataArray[count++] = vd1 ;
                vertexDataArray[count++] = vd2 ;
                vertexDataArray[count++] = vd3 ;
            }
            else {
                vertexDataArray[count++] = vd1 ;
                vertexDataArray[count++] = vd3 ;
                vertexDataArray[count++] = vd2 ;
            }
            
            
            if( !flip(vd3,vd4,vd1) ) {
                vertexDataArray[count++] = vd3 ;
                vertexDataArray[count++] = vd4 ;
                vertexDataArray[count++] = vd1 ;
            }
            else {
                vertexDataArray[count++] = vd3 ;
                vertexDataArray[count++] = vd1 ;
                vertexDataArray[count++] = vd4 ;
            }
            
        }
        
    }
    
    glBufferData(GL_ARRAY_BUFFER, _n*_n*6*sizeof(VertexData), vertexDataArray, GL_STATIC_DRAW);
    delete [] vertexDataArray ;
    checkError() ;
}






