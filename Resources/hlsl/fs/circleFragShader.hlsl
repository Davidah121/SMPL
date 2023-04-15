
cbuffer UniformData : register(b0)
{
    float2 circleCenter;
    float radius;
    float maxDisFromEdge;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

float4 main( VS_OUTPUT inputData) : SV_TARGET
{
    float2 pixelPosition = inputData.position.xy;
    float distVal = length(circleCenter - pixelPosition);
    float minOutDis = outerRadius-1;
    float maxOutDis = outerRadius;

    float minInDis = ((innerRadius-1) < 0) ? 0 : innerRadius-1;
    float maxInDis = ((innerRadius) < 0) ? 0 : innerRadius;
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
