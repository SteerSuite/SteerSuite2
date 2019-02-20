#version 400 core

uniform sampler2D texture1 ;
uniform sampler2D texture2 ;
uniform sampler2D texture3 ;


uniform float time ;
uniform int useTex ;
uniform float texCoordScale ;
uniform int useLighting ;


out vec4 fragColour;

in vec4 Colour; // Color from material
in vec4 VColour ;   // Color from vertices

in vec2 TexCoord ;
in vec3 AmbientDiffuseIntensity ;
in vec3 SpecularIntensity ;

// gl_FrontFacing returns true both for gl_FrontFacing and
// !gl_FrontFacing by some drives when accessed multiple times
// in the same shader
// The following code seems to work though

bool isFrontFacing(void) {
    vec2 f = vec2(gl_FrontFacing,0.0) ;
    if( f.x == 1.0) {
        return true ;
    }
    else {
        return false ;
    }
}

void main(void)
{

	vec4 texColour1,texColour2, texColour3, texColour = vec4(1.0f,1.0f,1.0f,1.0f) ;
	vec2 sTexCoord = texCoordScale*TexCoord ;

	if( useTex != 0 ) {
		vec4 texColour1 = texture(texture1,TexCoord);
		vec4 texColour2 = texture(texture2,TexCoord);
		vec4 texColour3 = texture(texture3,TexCoord);
	}
    
    switch( useTex ) {
        case 1: texColour = texColour1 ; break ;
        case 2: texColour = texColour2 ; break ;
        case 3: texColour = texColour3 ; break ;
        case 12: texColour = texColour1*texColour2 ; break ;
        case 13: texColour = texColour1*texColour3 ; break ;
        case 23: texColour = texColour2*texColour3 ; break ;
        case 123: texColour = texColour1*texColour2*texColour3 ; break ;
		case 100: texColour = VColour ; break ;
		default:  break;
    } ;
 
    if( useLighting == 1 ) fragColour = texColour*vec4(AmbientDiffuseIntensity,1.0)+vec4(SpecularIntensity,0.0f) ;
    else fragColour = texColour*Colour ;
 
    if( !isFrontFacing() )
       fragColour = vec4(0,1.0,1.0,1.0) ;

	fragColour.a = Colour.a;
}