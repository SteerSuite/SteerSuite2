//
//  geomObjects.h
//  cse3431Template
//
//  Created by Petros Faloutsos on 9/23/12.
//  Copyright (c) 2012 Petros Faloutsos. All rights reserved.
//

#ifndef cse3431Template_geomObjects_h
#define cse3431Template_geomObjects_h



#include "Globals.h"

#include "opengl.h"

using glm::vec4;
using glm::vec3 ;
using glm::vec2 ;




namespace Util {

	typedef struct VertexData {
		vec4 position;
		vec4 colour;
		vec3 normal;
		vec2 texCoord;
	} Vertexdata;

	class UTIL_API GLObject {
	public:
		GLObject() { _name = ""; };
		GLObject(std::string name) { _name = name; };
		virtual ~GLObject() ;
		virtual void Draw(void) { return; };
		std::string GetName() { return _name; };
		void getAttributeLocations(int  prog, int &pa,
			int &na, int &ca,
			int &ta);
		void SetBuffers(unsigned int prog);
		virtual void SetData(void){ exit(-1); std::cerr << "Object not child\n"; };
		void binVAO(void);

	protected:
		std::string _name;
		unsigned int  _vbo;
		unsigned int _vao;

	};


	class UTIL_API GLGrid : public GLObject {
	public:
		GLGrid() { return; };
		GLGrid(std::string name, float xOrigin, float zOrigin, int xGridSize, int zGridSize, int xNumCells, int zNumCells, GLuint prog);
		void Draw(void);
		void SetData(void);

	private:
		glm::vec2 _origin;
		glm::ivec2 _size;
		glm::ivec2 _numCells;

		int _n; //number of vertices
	};

	class UTIL_API GLLineStrip : public GLObject {
	public:
		GLLineStrip() { _n = 0;  return; };
		GLLineStrip(std::string name, GLuint prog);
		GLLineStrip(std::string name, Point *points, int n,  GLuint prog);

		void Draw(void);
		void SetData(void);
	private:
		int _n; // number of points
		Point *_p;
	};

	class UTIL_API GLLine : public GLObject {
	public:
		GLLine() { return; };
		GLLine(std::string name, GLuint prog);
		void Draw(void);
		void SetData(void);

		void Draw(const Point & p1, const Point & p2);
		void Draw(const Point & p1, const Point & p2, const Color &c);
		void Draw(const Point & p1, const Point & p2, const Color &c, float alpha);
		void Draw(const Point & p1, const Point & p2, glm::vec4 color);

	};

	class UTIL_API GLCircle : public GLObject {
	public:
		GLCircle() { _n = 0; return; };
		GLCircle(std::string name, int nSub, GLuint prog);
		void Draw(void);
		void SetData(void);
		int getN(void) { return _n; };
	private:
		int _n; // number of subdivision points
	};

	class UTIL_API GLDisk : public GLObject {
	public:
		GLDisk() { _n = 0; return; };
		GLDisk(std::string name, int nSub, GLuint prog);
		void Draw(void);
		void SetData(void);
		int getN(void) { return _n; };
	private:
		int _n; // number of subdivision points
	};

	class UTIL_API GLStar : public GLObject {
	public:
		GLStar() { _n = 0; return; };
		GLStar(std::string name, int nSub, GLuint prog);
		void Draw(void);
		void SetData(void);
		int getN(void) { return _n; };
	private:
		int _n; // number of subdivision points
	};

	class UTIL_API Sphere : public GLObject {
	public:
		Sphere() { _n = 0; return; }
		Sphere(std::string name, int nSub, GLuint prog);
		void Draw(void);

		void SetData(void);
		int getN(void) { return _n; };
	private:
		int _n; // number of subdivision in each paramater
	};

	class UTIL_API GLTriangle : public GLObject {
	public:
		GLTriangle() { return; };
		GLTriangle(std::string name, GLuint prog);
		void Draw(void);
		void SetData(void);
	};



	class UTIL_API Square : public GLObject {
	public:
		Square() { return; };
		Square(std::string name, GLuint prog);
		void Draw(void);
		void SetData(void);
	};

	class UTIL_API Quad : public GLObject {
	public:
		Quad() { return; };
		Quad(std::string name, GLuint prog);
		void Draw(void);
		void SetData(void);

		void Draw(const Point & a, const Point & b, const Point & c, const Point & d);
		void Draw(const Point & a, const Point & b, const Point & c, const Point & d, const Color &color);
		void Draw(const Point & a, const Point & b, const Point & c, const Point & d, glm::vec4 color);
	};

	class UTIL_API Cube : public GLObject {
	public:
		Cube() { return; };
		Cube(std::string name, GLuint prog);
		void Draw(void);
		void SetData(void);
	};

	class UTIL_API Cylinder : public GLObject {
	public:
		Cylinder() { _n = 0; return; };
		Cylinder(std::string name, int nSub, GLuint prog);
		void Draw(void);
		void SetData(void);
		int getN(void) { return _n; };
	private:
		int _n; // number of subdivision in each paramater
	};


	class UTIL_API Cone : public GLObject {
	public:
		Cone() { _n = 0; return; };
		Cone(std::string name, int nSub, GLuint prog);
		void Draw(void);
		void SetData(void);
		int getN(void) { return _n; };
	private:
		int _n; // number of subdivision in each paramater
	};



	class UTIL_API BezierPatch3 : public GLObject {
	public:
		BezierPatch3() { _n = 0; return; }
		BezierPatch3(std::string name, int nSub, GLfloat p[4][4][3], GLuint prog);
		void Draw(void);
		void SetData(void);
		int getN(void) { return _n; };
		void GetVertex(VertexData &vd, float u, float v);
		void SetControlPoints(GLfloat p[4][4][3]);

	private:
		int _n;
		glm::vec3 _cpoints[4][4];
	};


} // end name space UTIL

#endif