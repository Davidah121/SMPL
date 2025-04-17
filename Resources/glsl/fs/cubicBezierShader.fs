#version 420

out vec4 frag_color;
uniform vec2 bezierPoints[4];
uniform float innerDistance;
uniform float outerDistance;
uniform vec4 activeColor;

float sqr(float v)
{
    return v*v;
}
float cube(float v)
{
    return v*v*v;
}

vec2 cubicBezierAt(vec2 bezierP1, vec2 bezierP2, vec2 bezierP3, vec2 bezierP4, float t)
{
    return cube(1-t)*bezierP1 + 3*sqr(1-t)*t*bezierP2 + 3*(1-t)*sqr(t)*bezierP3 + cube(t)*bezierP4;
}
vec2 cubicBezierDerAt(vec2 bezierP1, vec2 bezierP2, vec2 bezierP3, vec2 bezierP4, float t)
{
    return 3*sqr(1-t)*(bezierP2 - bezierP1) + 6*(1-t)*t*(bezierP3 - beizerP2) + 3*sqr(t)*(bezierP4 - beizerP3);
}
vec2 cubicBezierDer2At(vec2 bezierP1, vec2 bezierP2, vec2 bezierP3, vec2 bezierP4, float t)
{
    return 6*(1-t)*(beizerP3 - 2*bezierP2 + bezierP1) + 6*t*(bezierP4 - 2*bezierP3 + beizerP2);
}

float findMinDistanceToBezier(vec2 bezierP1, vec2 bezierP2, vec2 bezierP3, vec2 bezierP4, vec2 p)
{
    float approxT = 0;
    float oldLen = length(bezierP1-p);
    for(int i=0; i<6; i++)
    {
        vec2 pointOnCurve = cubicBezierAt(bezierP1, bezierP2, bezierP3, bezierP4, (float)i / 6.0);
        float len = length(pointOnCurve - p);
        if(len < oldLen)
        {
            approxT = (float)i / 6.0;
            oldLen = len;
        }
    }

    //newton's method
    vec2 newBP1 = bezierP1-p;
    vec2 newBP2 = bezierP2-p;
    vec2 newBP3 = bezierP3-p;
    vec2 newBP4 = bezierP4-p;
    for(int i=0; i<5; i++)
    {
        vec2 Bt = cubicBezierAt(newBP1, newBP2, newBP3, newBP4, approxT);
        vec2 BtDer = cubicBezierDerAt(newBP1, newBP2, newBP3, newBP4, approxT);
        vec2 BtDer2 = cubicBezierDer2At(newBP1, newBP2, newBP3, newBP4, approxT);
        
        float lDer = 2*dot(Bt, BtDer);
        float lDer2 = 2*(dot(Bt, BtDer2) + dot(BtDer, BtDer));
        approxT -= lDer/lDer2;
    }

    return clamp(approxT, 0.0, 1.0);
}

void main()
{
    float approxT = findMinDistanceToBezier(bezierPoints[0], bezierPoints[1], bezierPoints[2], bezierPoints[3], gl_FragCoord);
    vec2 pointOnCurve = cubicBezierAt(approxT);
    vec2 toPointOnCurve = (pointOnCurve-gl_FragCoord);

    float len = length(toPointOnCurve);
    float alphaAdjustment = 1.0;
    float totalDistanceAllowed = outerDistance - innerDistance;
    if(len < outerDistance && len > innerDistance)
        alphaAdjustment = (totalDistanceAllowed - len) / totalDistanceAllowed;
    
    frag_color = vec4(activeColor.rgb, activeColor.a*alphaAdjustment);
}