attribute vec4		Position;
attribute vec2		TextureCoord;
attribute vec3		Normal;

uniform mat4		Projection;
uniform mat4		Modelview;
uniform mat4		Transform;

uniform vec3		LightPosition;
uniform vec4		LightColor;
uniform vec3		EyePosition;
uniform vec4		DiffuseMaterial;
uniform vec4		AmbientMaterial;
uniform vec4		SpecularMaterial;
uniform float		Shininess;

varying vec4		DestinationColor;
varying vec2		TexCoord;

void main(void)
{
	vec4 worldPosition = Transform * Position;
    gl_Position = Projection * Modelview * worldPosition;
	
    vec3 N = vec4( Transform * vec4( Normal, 1 ) ).xyz;
	N = normalize( N );
    vec3 L = normalize( LightPosition );
    vec3 E = normalize( EyePosition - worldPosition.xyz );
    vec3 H = normalize( L + E );

    float df = max( 0.0, dot( N, L ) );
    float sf = max( 0.0, dot( N, H ) );
    sf = pow( sf, Shininess );
	
	TexCoord = TextureCoord;

    DestinationColor = AmbientMaterial * DiffuseMaterial + df * DiffuseMaterial + sf * SpecularMaterial;
	DestinationColor *= LightColor;
}
