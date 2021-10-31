#version 420

in vec3 position;
in vec3 normal;

out vec3 vertNormal;
out vec3 vertPos;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
    vertNormal = (modelMatrix * vec4(normal, 0.0)).xyz;
    vertPos = (modelMatrix * vec4(position, 1.0)).xyz;
}
