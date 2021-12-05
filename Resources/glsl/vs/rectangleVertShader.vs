#version 420

in vec2 in_position;

out vec4 gl_Position;

uniform mat4 projectionMatrix;

void main()
{
    gl_Position = projectionMatrix * vec4(in_position, 0, 1);
}
