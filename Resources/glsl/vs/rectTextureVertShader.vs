#version 420

in vec2 in_position;
in vec2 in_texture;

out vec4 gl_Position;
out vec2 finalTextureCoords;

uniform mat4 projectionMatrix;

void main()
{
    gl_Position = projectionMatrix * vec4(in_position, 0, 1);
    finalTextureCoords = in_texture;
}
