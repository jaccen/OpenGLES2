precision highp float;												
													
uniform sampler2D			DiffuseTexture;
uniform mediump vec4		DiffuseMaterial;

varying lowp vec2			TexCoord;	
										
void main()
{
    gl_FragColor = DiffuseMaterial * texture2D( DiffuseTexture, TexCoord );
}