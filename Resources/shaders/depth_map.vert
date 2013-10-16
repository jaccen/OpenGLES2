attribute vec4		Position;

uniform mat4		Projection;
uniform mat4		Modelview;
uniform mat4		Transform;
uniform float		FarPlane;

varying float depth;

void main(void)
{
    gl_Position = Projection * Modelview * Transform * Position;
	depth = gl_Position.z / FarPlane;
}