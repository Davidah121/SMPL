#version 420

in vec2 finalTextureCoords;
out vec4 frag_color;

uniform vec4 activeColor;
uniform sampler2D textureID;

void main()
{
    frag_color = activeColor * texture2D(textureID, finalTextureCoords);
}