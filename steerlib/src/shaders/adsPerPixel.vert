#version 400 core

// Implement ADS per vertex lighting with a single light

// Vertex Attributes
layout (location = 0) in vec4 VertexPosition ;
layout (location = 1) in vec4 VertexColour ;
layout (location = 2) in vec3 VertexNormal ;
layout (location = 3) in vec2 VertexTexCoord ;

// Output variables to be interpolated by the rasterizer
out vec2 TexCoord ;
out vec3 NormalInEyeCoords ;
out vec4 PosInEyeCoords ;
out vec4 VColour ;

// Uniforms from main program
uniform float time;
uniform mat4 modelviewMat ;
uniform mat4 normalMat ;
uniform mat4 MVP ;


void main(void)
{
    
    VColour = VertexColour ; 
    
    PosInEyeCoords = modelviewMat*VertexPosition ;
    NormalInEyeCoords = normalize(vec3(normalMat*vec4(VertexNormal,0.0))) ;
    
   
    // sent out the vertex coord
    TexCoord = VertexTexCoord ;
  
    // Transform the vertex position from object to clip coordinates
	gl_Position = MVP * VertexPosition;
	
}

