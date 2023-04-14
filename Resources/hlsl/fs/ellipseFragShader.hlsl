
cbuffer UniformData : register(b0)
{
    float2 ellipseCenter;
    float2 dir1;
    float2 dir2;
    
    float innerDisFromEdge;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

float4 main( VS_OUTPUT inputData) : SV_TARGET
{
    float2 pixelPosition = inputData.position.xy;
    float2 toPoint = ellipseCenter - pixelPosition;
    float ang = atan2(toPoint.y, toPoint.x);
    float2 onEllipsePoint = dir1*cos(ang) + dir2*sin(ang); //Ellipse center not added for simplicity

    float distVal = length(toPoint);
    float shapeDis = length(onEllipsePoint);
    
    float maxOutDis = shapeDis;
    float maxInDis = shapeDis-innerDisFromEdge;
    float minOutDis = shapeDis-1;
    float minInDis = shapeDis-innerDisFromEdge-1;

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

    float4 outputColor = float4(inputData.color.rgb, inputData.color.a*alpha);
    return outputColor;
}
