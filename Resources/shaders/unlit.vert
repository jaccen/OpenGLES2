attribute vec4		Position;
attribute vec2		TextureCoord;

uniform mat4		Projection;
uniform mat4		Modelview;
uniform mat4		Transform;

varying vec2		vTexCoord;
varying float		vDepth;

void main(void)
{
	vTexCoord = TextureCoord;
	
    gl_Position = Projection * Modelview * Transform * Position;
	
	vDepth = gl_Position.z;
}