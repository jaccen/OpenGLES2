precision highp float;												
													
uniform sampler2D			DiffuseTexture;
uniform mediump vec4		DiffuseMaterial;

varying lowp vec2			TexCoord;	
varying lowp vec3			vNormal;
										
void main()
{
    vec4 color = DiffuseMaterial;
    gl_FragColor = color * texture2D( DiffuseTexture, TexCoord );
}