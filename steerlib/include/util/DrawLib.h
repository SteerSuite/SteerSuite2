//
// Copyright (c) 2009-2018 Brandon Haworth, Glen Berseth, Muhammad Usman, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//



#ifndef __UTIL_DRAWLIB_H__
#define __UTIL_DRAWLIB_H__

/// @file DrawLib.h
/// @brief Declares Util::DrawLib functionality, which provides convenient wrappers to many drawing functions.

#ifdef ENABLE_GUI

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#ifdef _WIN32
// on win32, there is an unfortunate conflict between exporting symbols for a
// dynamic/shared library and STL code.  A good document describing the problem
// in detail is http://www.unknownroad.com/rtfm/VisualStudio/warningC4251.html
// the "least evil" solution is just to simply ignore this warning.
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

#include <vector>

#include "util/Geometry.h"
#include "util/Color.h"

#include "glm/glm/glm.hpp"

#include "Globals.h"

#include "GLgeomObjects.h"




namespace Util {

	/**
	 * @brief A collection of useful functions for drawing and displaying graphics using openGL.
	 *
	 * #init() must be called explicitly by the user;  We cannot rely on static initialization
	 * because openGL may not be initialized.
	 *
	 * If you use this library, in particular the inlined functions, you may need to link openGL into your code.
	 *
	 * <b> Warning!!! </b>
	 * DrawLib is one of the few places where static functionality is used in %SteerLib.  This is
	 * necessary so that it can use openGL nicely across many modules.  Usually this is not a
	 * problem, because SteerLib is a shared library.  However, if you must statically link
	 * SteerLib or DrawLib for some reason, beware that multiple instances of DrawLib may occur,
	 * which may cause unexpected bugs like objects disappearing.
	 *
	 *
	 * @todo
	 *  - Consider making the display list functionality public.
	 *  - Fill-in some additional shape-drawing functionality
	 *  - Add support for text drawing functions in this library.
	 *
	 */
	class UTIL_API DrawLib {
	public:

		/// Call this after initializing openGL to initialize DrawLib;
		static void init();
		static void initUniforms(void);

		/// Call this after initializing openGL to delete resources DrawLib;
		static void finished();

		/// Sets background color
		static void setBackgroundColor(const Color& color);

		/// @name Camera and matrix functionality
		//@{
		static void setGLViewMatrix(float px,float py, float pz, float rx, float ry, float rz, float ux, float uy, float uz);
		static void setProjectionMatrix(glm::mat4 m);
		static void setMatricesFromStack(void);

		static void glTranslate(float x, float y, float z);
		static void glScale(float sx, float sy, float sz);
		static void glRotate(float rad, float x, float y, float z);
		static void glPushMatrix(void);
		static void glPopMatrix(void);
		static void glLoadIdentity(void);

		static void useShader(int index);

		//@}

		/// @name Lighting functionality
		//@{
		static void enableLights();
		static void disableLights();
		/// Updates the light positions; needs to be called every frame, right after setting the camera view.
		static void positionLights();

		//@}

		/// @name OpenGL wrappers
		/// @brief wrappers using our Util geometry data types;  If you use these in a module, you will also need to link with openGL.
		//@{
		static void glColor(const Color & color);
		static void setLight(glm::vec4 Position, glm::vec3 Ia, glm::vec3 Id, glm::vec3 Is);
		static void setColour(float c1, float c2, float c3, float a);
		static void setColour(float c1, float c2, float c3);
		static void useTexture(int t, float coordScale);
		static void useLighting(int t);

		static void setMaterial(glm::vec3 Ka, glm::vec3 Kd, glm::vec3 Ks, float a, float shininess);
		//@}

		/// @name Primitive shapes drawing functionality
		/// @brief make sure to set the color yourself, for example, using glColor3f(...) or DrawLib::glColor()
		//@{
		static void drawLine(const Point & startPoint, const Point & endPoint);
		static void drawLine(const Point & startPoint, const Point & endPoint, const Color & color);
		static void drawLineAlpha(const Point & startPoint, const Point & endPoint, const Color &color, float alpha);
		static void drawLine(const Point & startPoint, const Point & endPoint, const Color &color, float thickness);
		static void drawRay(const Ray & r, const Color & color);
		static void drawQuad(const Point & a, const Point & b, const Point & c, const Point & d);
		static void drawQuad(const Point & a, const Point & b, const Point & c, const Point & d, const Color &color);
		// static void drawCircle(const Point & center, float radius);
		static void drawBox(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax, bool fill=true);
		
		// static void drawOrientedBox(...);
		static void drawBoxWireFrame(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax);

		static void drawCylinder(const Point & pos, float radius, float ymin, float ymax);
		static void drawCylinder(const Point & pos, float radius, float ymin, float ymax, Color color);
		// static void drawCylinderWireFrame(const Point & pos, float radius, float ymin, float ymax);
		// static void drawSphere(const Point & center, float radius);
		static void drawCube(void);
		static void drawSquare(void);
		static void drawSphere(void);
		static void drawCone(void);
		static void drawCylinder(void);
		static void drawBezierPatch(BezierPatch3 *b);


		static void DrawLineStrip(Point *points,  int n);

		static GLGrid *MakeGLGrid(std::string name, float xOrigin, float zOrigin, int xGridSize, int zGridSize,
			int xNumCells, int zNumCells);
		//@}

		/// @name Composite shapes drawing functionality
		//@{
		static void GLdrawAgent(bool withTriangle=true);
		static void GLdrawAgentDot(void);
		static void drawStar(const Point & pos, const Vector & dir, float radius, const Color & color);
		static void drawHighlight(const Point & pos, const Vector & dir, float radius, const Color& color = Color(0.9f,0.9f,0.1f));
		static void drawFlag(const Point & loc, const Color& color = Color(0.9f, 0.0f, 0.0f), float scale = 1.0f);
		static void drawCircle(const Point & loc, const Color& color = Color(0.9f, 0.9f, 0.0f), float scale = 1.0f, int points = 12);
		// static void drawFlag(const Point & loc, const Color& color = Color(0.9f, 0.0f, 0.0f), float scale = 1.0f);
		static void drawAgentDisc(const Point & pos, const Vector & dir, float radius, const Color& color = Color(0.9f,0.4f,0.1f));
		static void drawAgentDisc(const Point & pos, float radius, const Color& color = Color(0.9f,0.4f,0.1f));
		static void drawAgentDisc(const Point & pos, const Vector & dir, const Vector & up, float radius, const Color& color);
		//@}

		/// @name Get OpenGL matrices
		//@{
		static glm::mat4 getModelViewMat(void);
		static glm::mat4 getProjectionMat(void);
	
		//@}

	private:
		/// @name Blocked functionality
		/// @brief these functions are not publically accessible / not implemented, to protect DrawLib from being instanced twice.
		//@{
		/// Initializes all drawing functionality, called only the first time _getInstance() is called.
		DrawLib();
		~DrawLib() {}
		DrawLib(const DrawLib & );  // not implemented, not copyable
		DrawLib& operator= (const DrawLib & );  // not implemented, not assignable
		//@}


	
		static bool _initialized;
	};

} // end namespace Util


#ifdef _WIN32
#pragma warning( pop )
#endif


#endif // ifdef ENABLE_GUI

#endif
