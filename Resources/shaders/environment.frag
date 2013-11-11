precision highp float;

uniform sampler2D			DiffuseTexture;
uniform sampler2D			SelfIlliminationTexture;

uniform samplerCube			EnvironmentMap;

uniform mediump vec4		LightColor;
uniform highp vec3			LightPosition;
uniform highp vec3			EyePosition;
uniform mediump vec4		DiffuseMaterial;
uniform mediump vec4		AmbientMaterial;
uniform mediump vec4		SelfIlliminationMaterial;
uniform mediump vec4		SpecularMaterial;
uniform mediump float		Shininess;
uniform mediump float		Reflectiveness;

varying lowp vec2			vTexCoord;
varying highp vec3			vNormal;
varying highp vec4			vWorldPos;

void main(void)
{
	highp vec3 ViewDir = EyePosition - vWorldPos.xyz;
	highp vec3 LightDir = LightPosition - vWorldPos.xyz;
	
	highp vec3 N = normalize( vNormal );
	highp vec3 E = normalize( ViewDir );
    highp vec3 L = normalize( LightDir );
    highp vec3 H = normalize( L + E );
	
    float df = max( 0.0, dot( N, L ) );
    float sf = max( 0.0, dot( N, H ) );
    sf = pow( max( 0.0, dot( N, H ) ), Shininess );
	
	vec3 reflectDir	= reflect( E, N );
	reflectDir = vec3( reflectDir.x, -reflectDir.y, reflectDir.z );
	vec4 environmentColor = textureCube( EnvironmentMap, reflectDir );
	
	vec4 texColor = texture2D( DiffuseTexture, vTexCoord ) * DiffuseMaterial;
	texColor = mix( texColor, environmentColor, Reflectiveness );
	vec4 pixelLitColor = AmbientMaterial * texColor + df * texColor + SpecularMaterial * sf;
    gl_FragColor = pixelLitColor * LightColor;
}