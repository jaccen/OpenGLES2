attribute vec4		Position;
attribute vec3		Normal;
attribute vec2		TextureCoord;

uniform mat4		Projection;
uniform mat4		Modelview;
uniform mat4		Transform;

varying vec2		TexCoord;
varying vec3		vNormal;

void main(void)
{
    vNormal = vec4( Transform * vec4( Normal, 0 ) ).xyz;
	vNormal = normalize( vNormal );
	
	TexCoord = TextureCoord;
	
    gl_Position = Projection * Modelview * Transform * Position;
}