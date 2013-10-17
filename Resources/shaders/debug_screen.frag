precision highp float;

uniform lowp vec4 Color;

varying lowp vec2 TexCoord;

void main()
{
    gl_FragColor = Color;
}