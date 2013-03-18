precision mediump float;

uniform sampler2D			DiffuseTexture;
uniform sampler2D			NormalTexture;
uniform mediump float		NormalFactor;

uniform mediump vec3		LightPosition;
uniform mediump vec3		EyePosition;
uniform mediump vec4		DiffuseMaterial;
uniform mediump vec4		AmbientMaterial;

varying lowp vec2			vTexCoord;
varying lowp vec3			vNormal;
varying highp vec4			vWorldPos;

void main(void)
{
	vec3 ViewDir = normalize( EyePosition - vWorldPos.xyz );
	vec3 LightDir = normalize( LightPosition - vWorldPos.xyz );
	vec3 N = vNormal + texture2D( NormalTexture, vTexCoord ).xyz * 0.5;
	vec3 E = normalize( ViewDir );
    vec3 L = normalize( LightDir );
    vec3 H = normalize(L + E);
	
    float df = max( 0.0, dot( N, L ) );
	
	vec4 texColor = texture2D( DiffuseTexture, vTexCoord );
    vec4 color = AmbientMaterial * texColor + df * texColor;
	
    gl_FragColor = color;
}