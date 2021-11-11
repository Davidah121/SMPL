#version 420

in vec2 parsedTextureCoords;

out vec4 fragColor;

uniform sampler2D textureID;

void main()
{
    fragColor = texture2D(textureID, parsedTextureCoords);
}
