
cbuffer UniformData : register(b0)
{
    float2 circleCenter;
    float radius;
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
    
    if(distVal > radius)
        discard;

    return inputData.color;
}
