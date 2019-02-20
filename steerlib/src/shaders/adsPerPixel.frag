#version 400 core

uniform sampler2D texture1 ;
uniform sampler2D texture2 ;
uniform sampler2D texture3 ;


uniform float time ;
uniform int useTex ;
uniform float texCoordScale ;
uniform int useLighting ;


out vec4 fragColour;


in vec4 VColour ;
in vec2 TexCoord ;
in vec3 NormalInEyeCoords ;
in vec4 PosInEyeCoords ;

struct LightInfo {
    vec4 Position ; // in eye coordinates
    vec3 Ia ; // ambient intensity
    vec3 Id ; // diffuse intensity
    vec3 Is ; // specular intensity
} ;

uniform LightInfo Light ;

struct MaterialInfo {
    vec3 Ka ; // ambient intensity
    vec3 Kd ; // diffuse intensity
    vec3 Ks ; // specular intensity
	float alpha ; // transparency
    float Shininess ;
} ;

uniform MaterialInfo Material ;

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

void ads(vec4 position, vec3 N, out vec3 ambientDiffuse, out vec3 specular)
{
    vec3 L = vec3(normalize(Light.Position-position)) ;
    vec3 V = normalize(-position.xyz) ;
    vec3 R = normalize(reflect(-L, N)) ;
    
    vec3 ambient = Light.Ia * Material.Ka;
    float LdotN = max(dot(N, L),0) ;
    vec3 diffuse = Light.Id*Material.Kd*LdotN ;
    ambientDiffuse = ambient+diffuse ;

    
    specular = vec3(0.0) ;
    if( LdotN > 0.0)
      specular = Light.Is*Material.Ks*pow(max(dot(R,V), 0.0), Material.Shininess) ;
}


void main(void)
{
	vec2 dummy = TexCoord ;


	vec4 texColour1,texColour2, texColour3, texColour = vec4(1.0f) ;
	vec2 sTexCoord = texCoordScale*TexCoord ;

	if( useTex != 0) {
		texColour1 = texture(texture1,sTexCoord);
		texColour2 = texture(texture2,sTexCoord);
		texColour3 = texture(texture3,sTexCoord);
    }
    vec3 AmbientDiffuseIntensity ;
    vec3 SpecularIntensity ;
    
    switch( useTex ) {
        case 1: texColour = texColour1 ; break ;
        case 2: texColour = texColour2 ; break ;
        case 3: texColour = texColour3 ; break ;
        case 12: texColour = texColour1*texColour2 ; break ;
        case 13: texColour = texColour1*texColour3 ; break ;
        case 23: texColour = texColour2*texColour3 ; break ;
        case 123: texColour = texColour1*texColour2*texColour3 ; break ;
		case 100: texColour = VColour ; break ;
        default: break;
    } ;
 
    if( useLighting == 1 ) {
		if( isFrontFacing()) {
			ads(PosInEyeCoords, NormalInEyeCoords, AmbientDiffuseIntensity, SpecularIntensity) ;
		}
		else {
			ads(PosInEyeCoords, -NormalInEyeCoords, AmbientDiffuseIntensity, SpecularIntensity) ;
		}
		fragColour = texColour*vec4(AmbientDiffuseIntensity,1.0)+vec4(SpecularIntensity,0.0f) ;
	}
	else fragColour = texColour*vec4(Material.Kd,1.0f) ;

	fragColour.a = Material.alpha;

}

