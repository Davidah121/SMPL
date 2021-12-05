#version 420

out vec4 frag_color;

uniform vec4 activeColor;

void main()
{
    frag_color = activeColor;
}