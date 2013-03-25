precision mediump float;

uniform sampler2D DiffuseTexture;

varying lowp vec4 DestinationColor;
varying highp vec2 TexCoord;

uniform mediump vec4		SelfIlliminationMaterial;
uniform sampler2D			SelfIlliminationTexture;

void main(void)
{
	float lightMap = texture2D( SelfIlliminationTexture, TexCoord ).r;
    vec4 texColor = texture2D( DiffuseTexture, TexCoord ) * DestinationColor;
	texColor = mix( texColor, SelfIlliminationMaterial, lightMap );
	gl_FragColor = texColor;
}