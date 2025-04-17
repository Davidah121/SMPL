#version 420

in vec2 pixelPosition;

out vec4 frag_color;

uniform vec4 activeColor;
uniform vec2 ellipseCenter;
uniform vec2 dir1;
uniform vec2 dir2;
uniform vec2 radi;
uniform float maxDisFromEdge;

void main()
{
    vec2 adjustedPos = pixelPosition - ellipseCenter;
    float d1Projection = dot(dir1, adjustedPos);
    float d2Projection = dot(dir2, adjustedPos);
    // vec2 innerScale = radi - vec2(maxDisFromEdge, maxDisFromEdge);

    //use normal ellipse equation -> (x^2 / rx^2) + (y^2 / ry^2) = 1
    float v = (d1Projection*d1Projection)/(radi.x*radi.x) + (d2Projection*d2Projection) / (radi.y*radi.y);
    
    if(v > 1)
        discard;
    
    //find distance to edge of shape
    vec2 toPoint = vec2(d1Projection, d2Projection);
    float distVal = length(toPoint);
    vec2 normToPoint = normalize(toPoint);

    float A = (normToPoint.x*normToPoint.x)/(radi.x*radi.x);
    float B = (normToPoint.y*normToPoint.y)/(radi.y*radi.y);
    float shapeDis = sqrt(1.0 / (A + B));

    float maxOutDis = shapeDis;
    float maxInDis = shapeDis-maxDisFromEdge;
    float minOutDis = shapeDis-1;
    float minInDis = shapeDis-maxDisFromEdge-1;
    
    float alpha = 1.0;
    
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