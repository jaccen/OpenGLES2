attribute vec4		Position;
attribute vec2		TextureCoord;

uniform mat4		Projection;
uniform mat4		Modelview;
uniform mat4		Transform;

varying vec2		vTexCoord;

void main(void)
{
	vTexCoord = TextureCoord;
	
    gl_Position = Projection * Modelview * Transform * Position;
}