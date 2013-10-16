precision mediump float;

uniform sampler2D			DiffuseTexture;
uniform sampler2D			DepthMapTexture;
uniform mediump vec4		DiffuseMaterial;

uniform vec2				Scale;

varying lowp vec2			vTexCoord;
varying highp float			vDepth;

void main(void)
{
	gl_FragColor = texture2D( DiffuseTexture, vTexCoord * Scale ) * DiffuseMaterial;
}