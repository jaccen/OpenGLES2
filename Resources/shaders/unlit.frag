precision mediump float;

uniform sampler2D			DiffuseTexture;
uniform mediump vec4		DiffuseMaterial;

uniform vec2				Scale;

varying lowp vec2			vTexCoord;

void main(void)
{
    gl_FragColor = texture2D( DiffuseTexture, vTexCoord * Scale ) * DiffuseMaterial;
}