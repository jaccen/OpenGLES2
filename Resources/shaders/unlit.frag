precision mediump float;

uniform sampler2D			DiffuseTexture;

uniform mediump vec3		LightPosition;
uniform mediump vec4		DiffuseMaterial;
uniform mediump vec4		AmbientMaterial;
uniform mediump vec4		SpecularMaterial;
uniform mediump float		Shininess;
uniform mediump float		Glossiness;

varying lowp vec2			vTexCoord;

void main(void)
{
    gl_FragColor = texture2D( DiffuseTexture, vTexCoord ) * DiffuseMaterial;
}