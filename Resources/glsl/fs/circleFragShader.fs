#version 420

in vec2 pixelPosition;

out vec4 frag_color;

uniform vec4 activeColor;
uniform vec2 circleCenter;
uniform float radius;
uniform float maxDisFromEdge;

void main()
{
    float distVal = length(circleCenter - pixelPosition);
    float minOutDis = radius-1;
    float maxOutDis = radius;

    float minInDis = radius-maxDisFromEdge-1;
    float maxInDis = radius-maxDisFromEdge;
    float alpha = 1.0;

    if(distVal > maxOutDis)
        discard;
    if(distVal < minInDis)
        discard;

    if(distVal > minOutDis)
    {
        alpha = (maxOutDis - distVal);
    }
    else if(distVal < maxInDis)
    {
        alpha = (distVal - minInDis);
    }
    
    frag_color = vec4(activeColor.rgb, activeColor.a*alpha);
}