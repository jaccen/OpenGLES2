precision mediump float;

uniform sampler2D			DiffuseTexture;
uniform sampler2D			SpecularMapTexture;

uniform mediump vec4		LightColor;
uniform mediump vec3		LightPosition;
uniform mediump vec3		EyePosition;
uniform mediump vec4		DiffuseMaterial;
uniform mediump vec4		AmbientMaterial;
uniform mediump vec4		SelfIlliminationMaterial;
uniform mediump vec4		SpecularMaterial;
uniform mediump float		Shininess;

uniform vec2				Scale;

varying lowp vec2			vTexCoord;
varying highp vec3			vNormal;
varying highp vec4			vWorldPos;

void main(void)
{
	vec2 texCoord = vTexCoord * Scale;
	
	vec3 ViewDir = normalize( EyePosition - vWorldPos.xyz );
	vec3 LightDir = normalize( LightPosition - vWorldPos.xyz );
	
	vec3 N = normalize( vNormal );
	vec3 E = normalize( ViewDir );
    vec3 L = normalize( LightDir );
    vec3 H = normalize( L + E );
	
    float df = max( 0.0, dot( N, L ) );
    float sf = max( 0.0, dot( N, H ) );
    sf = pow( max( 0.0, dot( N, H ) ), Shininess );
	float specAmt = texture2D( SpecularMapTexture, texCoord ).r;
	
	vec4 texColor = texture2D( DiffuseTexture, texCoord );
	
	vec4 pixelLitColor = AmbientMaterial * texColor + df * texColor + SpecularMaterial * sf * specAmt;
    gl_FragColor = pixelLitColor * LightColor;
}