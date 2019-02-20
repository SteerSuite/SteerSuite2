/*
 *  opengl.h
 *  
 *
 *  Created by Petros Faloutsos on 3/26/08. Used code from openGL programming guide 8th edition
 *
 */
#ifndef _opengl_h_
#define _opengl_h_

#define GLFW_INCLUDE_GL3
#define GLFW_NO_GLU

#ifdef __APPLE__
#include "glew/include/GL/glew.h"
#include "glfw3/include/GLFW/glfw3.h"
#else
#include "glew/include/GL/glew.h"
#include "glfw3/include/GLFW/glfw3.h"
#endif

#include <iostream>
namespace Util {

static const char*
ErrorString( GLenum error )
{
    const char*  msg="";
    switch( error ) {
#define Case( Token )  case Token: msg = #Token; break;
            Case( GL_NO_ERROR );
            Case( GL_INVALID_VALUE );
            Case( GL_INVALID_ENUM );
            Case( GL_INVALID_OPERATION );
            Case( GL_STACK_OVERFLOW );
            Case( GL_STACK_UNDERFLOW );
            Case( GL_OUT_OF_MEMORY );
#undef Case
    }
    
    return msg;
}

//----------------------------------------------------------------------------

static
void _CheckError(const char* file, int line)
{
    GLenum  error;
    
    while ((error = glGetError()) != GL_NO_ERROR ) {
        if( error == 0 )
        {
            std::cerr << "glGetError failed!\n" ;
            continue ;
            
        }
        fprintf( stderr, "File %s line %d: OpenGL Error: %s\n", file, line, ErrorString(error) );
    }
    
}

#define checkError()  _CheckError( __FILE__, __LINE__ )

}
#endif
