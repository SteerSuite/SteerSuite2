#version 150
precision highp float;

uniform float time ;

in vec4 Colour;
in vec2 TexCoord ;
out vec4 fragColour;


void main(void)
{
    fragColour = Colour;
   // if( !gl_FrontFacing )
   //     fragColour = vec4(0.0,1.0,1.0,1.0) ;
}