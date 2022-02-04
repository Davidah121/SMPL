
cbuffer UniformData : register(b0)
{
    float4 activeColor;
    float4x4 projectionMatrix;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VS_OUTPUT main( float2 in_position : POSITION )
{
    VS_OUTPUT output;
    output.position = mul( float4(in_position, 0, 1), projectionMatrix);
    output.color = activeColor;
    return output;
}
