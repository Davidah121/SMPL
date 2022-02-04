
cbuffer UniformData : register(b0)
{
    float4x4 projectionMatrix;
};

cbuffer ModelMatrix : register(b1)
{
    float4x4 modelMatrix;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

VS_OUTPUT main( float2 in_position : POSITION, float2 in_texture : TEXCOORD )
{
    VS_OUTPUT output;
    output.position = mul( mul( float4(in_position, 0, 1), modelMatrix ), projectionMatrix );
    output.texcoord = in_texture;
    return output;
}
