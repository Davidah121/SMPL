#version 420

in vec2 pixelPosition;

out vec4 frag_color;

uniform vec4 activeColor;
uniform vec2 circleCenter;
uniform float radius;

void main()
{
    float distVal = length(circleCenter - pixelPosition);
    
    if(distVal > radius)
        discard;
    else
        frag_color = activeColor;
}