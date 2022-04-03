#version 420

in vec2 pixelPosition;

out vec4 frag_color;

uniform vec4 activeColor;
uniform vec2 circleCenter;
uniform float innerRad;
uniform float outerRad;

void main()
{
    float distVal = length(circleCenter - pixelPosition);
    float minOutDis = outerRad-1;
    float maxOutDis = outerRad;

    float minInDis = ((innerRad-1) < 0) ? 0 : innerRad-1;
    float maxInDis = ((innerRad) < 0) ? 0 : innerRad;
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