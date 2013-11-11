precision mediump float;

uniform sampler2D			DiffuseTexture;
uniform sampler2D			SelfIlluminationTexture;

uniform mediump vec4		LightColor;
uniform mediump vec3		LightPosition;
uniform mediump vec3		EyePosition;
uniform mediump vec4		DiffuseMaterial;
uniform mediump vec4		AmbientMaterial;
uniform mediump vec4		SpecularMaterial;
uniform mediump float		Shininess;

varying lowp vec2			vTexCoord;
varying lowp vec3			vNormal;
varying highp vec4			vWorldPos;

void main(void)
{
	vec3 ViewDir = normalize( EyePosition - vWorldPos.xyz );
	vec3 LightDir = normalize( LightPosition - vWorldPos.xyz );
	
	vec3 N = normalize( vNormal );
	vec3 E = normalize( ViewDir );
    vec3 L = normalize( LightDir );
    vec3 H = normalize( L + E );
	
    float df = max( 0.0, dot( N, L ) );
    float sf = max( 0.0, dot( N, H ) );
	vec4 selfIllum = texture2D( SelfIlluminationTexture, vec2( vTexCoord.x, -vTexCoord.y ) );
    sf = pow( max( 0.0, dot( N, H ) ), Shininess );
	
	vec4 texColor = texture2D( DiffuseTexture, vTexCoord ) * DiffuseMaterial;
	vec4 pixelLitColor = AmbientMaterial * texColor + df * texColor + SpecularMaterial * sf;
    gl_FragColor = pixelLitColor * LightColor + selfIllum;
}