#version 420

uniform sampler2D inTexture;
in vec2 textureCoord;

out vec4 fragColor;

void main()
{
    fragColor = texture2D(inTexture, textureCoord);
}
