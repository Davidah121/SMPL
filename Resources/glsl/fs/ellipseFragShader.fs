#version 420

in vec2 pixelPosition;

out vec4 frag_color;

uniform vec4 activeColor;
uniform vec2 ellipseCenter;
uniform vec2 dir1;
uniform vec2 dir2;
uniform float maxDisFromEdge;

void main()
{
    vec2 toPoint = pixelPosition - ellipseCenter;
    float ang = atan(toPoint.y, toPoint.x);
    vec2 onEllipsePoint = dir1*cos(ang) + dir2*sin(ang); //Ellipse center not added for simplicity

    float distVal = length(toPoint);
    float shapeDis = length(onEllipsePoint);

    float maxOutDis = shapeDis;
    float maxInDis = shapeDis-maxDisFromEdge;
    float minOutDis = shapeDis-1;
    float minInDis = shapeDis-maxDisFromEdge-1;
    
    float alpha = 1.0;

    if(distVal > maxOutDis)
        discard;
    // if(distVal < minInDis)
    //     discard;
    
    if(distVal > minOutDis)
    {
        alpha = (maxOutDis - distVal);
    }
    // else if(distVal < maxInDis)
    // {
    //     alpha = (distVal - minInDis);
    // }
    
    frag_color = vec4(activeColor.rgb, activeColor.a*alpha);
}