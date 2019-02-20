//
//  program.cpp
//  cse3431Template
//
//  Created by Petros Faloutsos on 13-09-26.
//  Copyright (c) 2013 Petros Faloutsos. All rights reserved.
//


#include "program.h"

using namespace Util;
using std::cerr ;

Program::~Program(void)
{
    glDeleteShader(_prog) ;
}

void Program::printActiveUniforms(void)
{
	GLint numActiveUnifs = 0;

	GLint maxLength ;
	glGetProgramiv(_prog, GL_ACTIVE_UNIFORMS, &numActiveUnifs);
	glGetProgramiv(_prog, GL_ACTIVE_UNIFORM_MAX_LENGTH , &maxLength) ;

	GLchar *name = (GLchar *) malloc(maxLength) ;
	GLuint location ;
	GLsizei written, size ;
	GLenum type ;

	GLint maxNumVertexUnif = 0;
	glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS,&maxNumVertexUnif);
    printf("GL_MAX_VERTEX_UNIFORM_COMPONENS %d\n", maxNumVertexUnif);
	GLint maxNumFragUnif = 0;
	glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,&maxNumFragUnif);
	printf("GL_MAX_FRAGMENT_UNIFORM_COMPONENTS %d\n", maxNumFragUnif);
	printf("Uniform: Index | Name \n") ;
	for(int i = 0; i < numActiveUnifs; ++i)
	{
		glGetActiveUniform(_prog, i, maxLength, &written, &size, &type, name);
		location = glGetUniformLocation(_prog, name) ;
		printf("%-5d | %s\n", location, name) ;
	}
}



void Program::printActiveAttributes(void)
{
	GLint numActiveAttribs = 0;

	GLint maxLength ;
	glGetProgramiv(_prog, GL_ACTIVE_ATTRIBUTES, &numActiveAttribs);
	glGetProgramiv(_prog, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH , &maxLength) ;

	GLchar *name = (GLchar *) malloc(maxLength) ;
	GLuint location ;
	GLsizei written, size ;
	GLenum type ;

	GLint maxNumAtt = 0;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxNumAtt);
    printf("GL_MAX_VERTEX_ATTRIBS %d\n", maxNumAtt);
	printf("Att: Index | Name \n") ;
	for(int i = 0; i < numActiveAttribs; ++i)
	{
		glGetActiveAttrib(_prog, i, maxLength, &written, &size, &type, name);
		location = glGetAttribLocation(_prog, name) ;
		printf("%-5d | %s\n", location, name) ;
	}
}

// Create a NULL-terminated string by reading the provided file
char*
Program::readShaderSource(const char* shaderFile)
{
    FILE *filePointer;
    char *content = NULL;
    
    long int count=0;
    
    if (shaderFile != NULL) {
        filePointer = fopen(shaderFile,"rt");
        
        if (filePointer != NULL) {
            
            fseek(filePointer, 0, SEEK_END);
            count = ftell(filePointer);
            rewind(filePointer);
            
            if (count > 0) {
                content = (char *)malloc(sizeof(char) * (count+1));
                count = fread(content,sizeof(char),count,filePointer);
                content[count] = '\0';
            }
            fclose(filePointer);
        }
    }
    return content;
    
}

void bindAttributes(GLuint pid)
{
    glBindAttribLocation(pid,0, "VertexPosition") ;
    glBindAttribLocation(pid,1, "VertexColour") ;
    glBindAttribLocation(pid,2, "VertexNormal") ;
    glBindAttribLocation(pid,3, "VertexTexCoord") ;
}

// Create a GLSL program object from vertex and fragment shader files
GLuint Program::InitShaderPair(const char* vShaderFile, const char* fShaderFile, const char* outputAttributeName) {
    struct Shader {
        const char*  filename;
        GLenum       type;
        GLchar*      source;
    }  shaders[2] = {
        { vShaderFile, GL_VERTEX_SHADER, NULL },
        { fShaderFile, GL_FRAGMENT_SHADER, NULL }
    };
    
	shaders[0].source = NULL ;
	shaders[1].source = NULL ;
    GLuint program = glCreateProgram();
    
    for ( int i = 0; i < 2; ++i ) {
        Shader& s = shaders[i];
        s.source = readShaderSource( s.filename );
		
        if ( shaders[i].source == NULL ) {
            std::cerr << "Failed to read " << s.filename << std::endl;
            exit( EXIT_FAILURE );
        }
        GLuint shader = glCreateShader( s.type );
        glShaderSource( shader, 1, (const GLchar**) &s.source, NULL );
        glCompileShader( shader );
        
        GLint  compiled;
        glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
        if ( !compiled ) {
            std::cerr << s.filename << " failed to compile:" << std::endl;
            GLint  logSize;
            glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logSize );
            char* logMsg = new char[logSize];
            glGetShaderInfoLog( shader, logSize, NULL, logMsg );
            std::cerr << logMsg << std::endl;
            delete [] logMsg;
            
            exit( EXIT_FAILURE );
        }
        
        delete [] s.source;
        
        glAttachShader( program, shader );
    }
    
    // Bind the attribute and fragment locations
    glBindFragDataLocation(program, 0, outputAttributeName);
    bindAttributes(program) ;
    
    /* link  and error check */
    glLinkProgram(program);
    
    GLint  linked;
    glGetProgramiv( program, GL_LINK_STATUS, &linked );
    if ( !linked ) {
        std::cerr << "Shader program failed to link" << std::endl;
        GLint  logSize;
        glGetProgramiv( program, GL_INFO_LOG_LENGTH, &logSize);
        char* logMsg = new char[logSize];
        glGetProgramInfoLog( program, logSize, NULL, logMsg );
        std::cerr << logMsg << std::endl;
        delete [] logMsg;
        
        exit( EXIT_FAILURE );
    }
    
    std::cout << "Linked " << vShaderFile << " " << fShaderFile << std::endl ;


    /* use program object */
    glUseProgram(program);
    
    return program;
}


GLuint Program::loadShaderPair(std::string vertShader, std::string fragShader ){
    
	const char *fragOut = "fragColour";
    _prog = InitShaderPair(vertShader.c_str(), fragShader.c_str(), fragOut);

	GLint texCoordAttribute = glGetAttribLocation(_prog, "VertexTexCoord");
	if (texCoordAttribute < 0) {
		cerr << "Shader did not contain the 'VertexTexCoord' attribute." << std::endl;
	}

#ifdef _DEBUG
	printActiveUniforms() ;
	printActiveAttributes() ;
#endif 
    return _prog ;
}

//Sets a uniform matrix in program prog
void Program::setMatrix4f(const std::string name, glm::mat4 &m)
{
    checkError() ;
    GLuint loc = glGetUniformLocation(_prog, name.c_str()) ;
    if ((int) loc < 0) {
        std::cerr << "Program::setMatrix4f: Shader did not contain the uniform: " << name <<std::endl;
	}
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m)) ;
    checkError();
    
}

void Program::setFloat(const std::string name, float v)
{
    GLuint loc = glGetUniformLocation(_prog, name.c_str()) ;
    if ((int) loc < 0) {
        std::cerr << "Program::setFloat: Shader did not contain the uniform: " << name <<std::endl;
	}
    glUniform1f(loc, v) ;
    checkError();
}

void Program::setInt(const std::string name, int v)
{
    checkError();
    GLuint loc = glGetUniformLocation(_prog, name.c_str()) ;
    if ((int) loc < 0) {
        std::cerr << "Program::setInt: Shader did not contain the uniform: " << name <<std::endl;
	}
    glUniform1i(loc, v) ;
    checkError();
}

void Program::setFloat3(const std::string name, float v1, float v2, float v3)
{
    GLuint loc = glGetUniformLocation(_prog, name.c_str()) ;
    if ((int) loc < 0) {
        std::cerr << "Program::setFloat3: Shader did not contain the uniform: " << name <<std::endl;
	}
    glUniform3f(loc, v1,v2,v3) ;
    checkError();
}

void Program::setFloat4(const std::string name, float v1, float v2, float v3, float v4)
{
    GLuint loc = glGetUniformLocation(_prog, name.c_str()) ;
    if ((int) loc < 0) {
        std::cerr << "Program::setFloat4: Shader did not contain the uniform: " << name <<std::endl;
	}
    glUniform4f(loc, v1,v2,v3,v4) ;
    checkError();
}

GLuint Shaders::loadShaderPair(std::string name)
{
    std::string vertShader = _dir+ name + ".vert" ;
    std::string fragShader = _dir+ name + ".frag" ;
    
    Program *p = new Program();
    GLuint pid = p->loadShaderPair(vertShader, fragShader) ;
    
    if( pid > 0 ) _progs.push_back(p) ;
    
    //if( _progs.size() == 1) this->use(0) ;
    
    return pid ;
}

GLuint Shaders::use(int n)
{
    Program *p = _progs.at(n) ;
    glUseProgram(p->id()) ;
    _activeIndex = n ;
    _activeProgram = p ;
    
    return p->id() ;
}

