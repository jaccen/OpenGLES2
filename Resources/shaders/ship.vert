attribute vec4		Position;
attribute vec3		Normal;
attribute vec2		TextureCoord;

uniform mat4		Projection;
uniform mat4		Modelview;
uniform mat4		Transform;

varying vec2		vTexCoord;
varying vec3		vNormal;
varying vec4		vWorldPos;

void main(void)
{
    vNormal = vec4( Transform * vec4( Normal, 0 ) ).xyz;
	
	vTexCoord = TextureCoord;
	
	vWorldPos = Transform * Position;
    gl_Position = Projection * Modelview * vWorldPos;
}