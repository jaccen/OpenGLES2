precision highp float;												
													
uniform sampler2D			DiffuseTexture;
uniform mediump vec4		DiffuseMaterial;

varying lowp vec2			TexCoord;	
varying lowp vec3			vNormal;
										
void main()
{
	float alpha = texture2D( DiffuseTexture, TexCoord ).a;
    vec4 color = DiffuseMaterial;
	color.a = alpha;

	// For debugging, draws red background
    gl_FragColor = mix( vec4(1,0,0,0.5), DiffuseMaterial, alpha );
    
	//gl_FragColor = color;
}