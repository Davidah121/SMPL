#version 420

in vec2 position;
in vec2 textureCoords;

out vec2 parsedTextureCoords;

void main()
{
    gl_Position = vec4(position, 0, 1);
    parsedTextureCoords = textureCoords;
}
