static const char* SimpleFragmentShader = STRINGIFY(

uniform sampler2D DiffuseTexture;

varying lowp vec4 DestinationColor;
varying highp vec2 TexCoord;

void main(void)
{
    gl_FragColor = texture2D( DiffuseTexture, TexCoord ) * DestinationColor;
}

);
