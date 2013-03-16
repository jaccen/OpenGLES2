attribute vec4		Position;
attribute vec3		Normal;
attribute vec2		TextureCoord;

uniform mat4		Transform;
uniform mat4		ScreenTransform;

varying vec2		TexCoord;
										
varying vec3		vNormal;
												  
void main()
{
    vNormal = Normal;
	
	TexCoord = TextureCoord;
	
    gl_Position = ScreenTransform * Transform * Position;
}