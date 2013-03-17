attribute vec4		Position;
attribute vec3		Normal;
attribute vec2		TextureCoord;

uniform mat4		Projection;
uniform mat4		Modelview;
uniform mat4		Transform;

uniform vec4		DiffuseMaterial;

varying vec4		DestinationColor;
varying vec2		TexCoord;

void main(void)
{
	TexCoord = TextureCoord;
	
	vec3 n = normalize( Normal );

    DestinationColor = DiffuseMaterial;
	
    gl_Position = Projection * Modelview * Transform * Position;
}
