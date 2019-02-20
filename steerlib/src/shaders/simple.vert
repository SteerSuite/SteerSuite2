#version 150

uniform float time;
uniform mat4 modelviewMat ;
uniform mat4 normalMat ;
uniform mat4 projMat ;
uniform vec4 colour ;

in vec4 VertexPosition ;
in vec4 VertexColour ;
in vec3 VertexNormal ;
in vec2 VertexTexCoord ;


out vec4 Colour;
out vec2 TexCoord ;

void main (void)
{
    vec4 gColor = colour ;
    Colour = VertexColour ;
    //Colour = colour ;
    TexCoord = VertexTexCoord ;
   
    vec3 nn = normalize(vec3(normalMat*vec4(VertexNormal,0.0))) ;
   
    gl_Position = projMat*modelviewMat*VertexPosition ;
    
}