attribute vec4		Position;
attribute vec2		TextureCoord;

uniform mat4		Transform;
uniform mat4		ScreenTransform;

varying vec2		TexCoord;
												  
void main()
{
	TexCoord = TextureCoord;
	
    gl_Position = ScreenTransform * Transform * Position;
}