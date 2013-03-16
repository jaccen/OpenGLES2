precision mediump float;

uniform sampler2D			DiffuseTexture;
uniform sampler2D			NormalTexture;
uniform mediump float		NormalFactor;

uniform mediump vec3		LightPosition;
uniform mediump vec4		DiffuseMaterial;
uniform mediump vec4		AmbientMaterial;
uniform mediump vec4		SpecularMaterial;
uniform mediump float		Shininess;
uniform mediump float		Glossiness;

varying lowp vec2			TexCoord;
varying lowp vec3			vNormal;

void main(void)
{
	vec3 N = vNormal + texture2D( NormalTexture, TexCoord ).xyz * 0.5;
	vec3 E = vec3( 0, 0, 1 ); // TODO: Fix this
    vec3 L = normalize( LightPosition );
    vec3 H = normalize(L + E);
	
    float df = max( 0.0, dot( N, L) );
    float sf = max( 0.0, dot( N, H) );
    sf = pow(sf, Shininess);
	sf = 0.0;
	
	vec4 texColor = texture2D( DiffuseTexture, TexCoord );
	vec4 specColor = sf * mix( texColor, SpecularMaterial, Glossiness );
    vec4 color = AmbientMaterial * texColor + df * texColor + specColor;
	
    gl_FragColor = color;
}