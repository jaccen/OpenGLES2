precision highp float;												
													
uniform sampler2D			DiffuseTexture;
uniform mediump vec4		DiffuseMaterial;

varying lowp vec2			TexCoord;	
varying lowp vec3			vNormal;
										
void main()
{
    vec4 color = DiffuseMaterial * texture2D( DiffuseTexture, TexCoord );
	
	color.r = color.r * 0.393 + color.g * 0.769 + color.b * 0.189;
	color.g = color.r * 0.349 + color.g * 0.686 + color.b * 0.168;
	color.b = color.r * 0.272 + color.g * 0.534 + color.b * 0.131;
	
	gl_FragColor = color;
}