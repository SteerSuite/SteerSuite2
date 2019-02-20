#version 400 core

// Implement ADS per vertex lighting with a single light

// Vertex Attributes
layout (location = 0) in vec4 VertexPosition ;
layout (location = 1) in vec4 VertexColour ;
layout (location = 2) in vec3 VertexNormal ;
layout (location = 3) in vec2 VertexTexCoord ;

// Output variables to be interpolated by the rasterizer
out vec4 Colour ;
out vec2 TexCoord ;
out vec3 AmbientDiffuseIntensity ;
out vec3 SpecularIntensity ;
out vec4 VColour ;

// Uniforms from main program
uniform mat4 modelviewMat ;
uniform mat4 normalMat ;
uniform mat4 MVP ;
uniform int useLighting ;

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
	float alpha;
    float Shininess ;
} ;


uniform MaterialInfo Material ;

void ads(vec4 position, vec3 N, out vec3 ambientDiffuse, out vec3 specular)
{
    vec3 L = vec3(normalize(Light.Position-position)) ;
    vec3 V = normalize(-position.xyz) ;
    vec3 R = reflect(-L, N) ;
    
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
    
    VColour = VertexColour ; // to eliminate warnings
    
    Colour = vec4(Material.Kd,Material.alpha) ; // In case no lighting is used
    vec4 pv = modelviewMat*VertexPosition ;
    vec3 tn = normalize(vec3(normalMat*vec4(VertexNormal,0.0))) ;
    
	if( useLighting ==1 ) {
		// compute ads lighting
		ads(pv,tn, AmbientDiffuseIntensity, SpecularIntensity) ;
	}
     
   
    // sent out the vertex coord
    TexCoord = VertexTexCoord ;
	vec2 test = VertexTexCoord*3 ;
  
    // Transform the vertex position from object to clip coordinates
	gl_Position = MVP * VertexPosition;
	
}

