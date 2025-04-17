#version 420

out vec4 frag_color;
uniform vec2 bezierPoints[3];
uniform float innerDistance;
uniform float outerDistance;
uniform vec4 activeColor;

float sqr(float v)
{
    return v*v;
}

vec2 quadBezierAt(vec2 bezierP1, vec2 bezierP2, vec2 bezierP3, float t)
{
    return sqr(1-t)*bezierP1 + 2*(1-t)*t*bezierP2 + sqr(t)*bezierP3;
}
vec2 quadBezierDerAt(vec2 bezierP1, vec2 bezierP2, vec2 bezierP3, float t)
{
    return 2*(1-t)*(bezierP2-bezierP1) + 2*t*(bezierP3-bezierP2);
}
vec2 quadBezierDer2At(vec2 bezierP1, vec2 bezierP2, vec2 bezierP3, float t)
{
    return 2*(bezierP1 - 2*bezierP2 + bezierP3);
}

float findMinDistanceToBezier(vec2 bezierP1, vec2 bezierP2, vec2 bezierP3, vec2 p)
{
    float approxT = 0;
    float oldLen = length(bezierP1-p);
    for(int i=0; i<6; i++)
    {
        vec2 pointOnCurve = quadBezierAt(bezierP1, bezierP2, bezierP3, (float)i / 6.0);
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
    for(int i=0; i<5; i++)
    {
        vec2 Bt = quadBezierAt(newBP1, newBP2, newBP3, approxT);
        vec2 BtDer = quadBezierDerAt(newBP1, newBP2, newBP3, approxT);
        vec2 BtDer2 = quadBezierDer2At(newBP1, newBP2, newBP3, approxT);
        
        float lDer = 2*dot(Bt, BtDer);
        float lDer2 = 2*(dot(Bt, BtDer2) + dot(BtDer, BtDer));
        approxT -= lDer/lDer2;
    }

    return clamp(approxT, 0.0, 1.0);
}

void main()
{
    float approxT = findMinDistanceToBezier(bezierPoints[0], bezierPoints[1], bezierPoints[2], gl_FragCoord);
    vec2 pointOnCurve = quadBezierAt(approxT);
    vec2 toPointOnCurve = (pointOnCurve-gl_FragCoord);

    float len = length(toPointOnCurve);
    float alphaAdjustment = 1.0;
    float totalDistanceAllowed = outerDistance - innerDistance;
    if(len < outerDistance && len > innerDistance)
        alphaAdjustment = (totalDistanceAllowed - len) / totalDistanceAllowed;
    
    frag_color = vec4(activeColor.rgb, activeColor.a*alphaAdjustment);
}