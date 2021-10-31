#version 420

in vec3 position;
in vec3 color;

out vec4 vertColor;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
    vertColor = vec4(color, 1.0);
}
