#version 420

in vec3 position;
in vec2 tex;
in vec3 normal;

out vec4 gl_Position;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

out vec3 lPos;
out vec2 lTex;
out vec3 lNormal;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * vec4(position, 1.0);
    lPos = position;
    lTex = tex;
    lNormal = normal;
}
