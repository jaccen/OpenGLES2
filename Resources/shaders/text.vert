attribute vec4		Position;
attribute vec3		Normal;
attribute vec2		TextureCoord;

uniform mat4		Transform;
uniform mat4		ScreenTransform;
uniform mat4		SourceTransform;

varying vec2		TexCoord;
										
varying vec3		vNormal;
												  
void main()
{
    vNormal = Normal;
	
	vec4 offsetTexCoords = SourceTransform * vec4( TextureCoord.x, TextureCoord.y, 0.0, 1.0 );
	TexCoord = offsetTexCoords.xy;
	
    gl_Position = ScreenTransform * Transform * Position;
}