cbuffer UniformData : register(b0)
{
    float4 activeColor;
};

Texture2D Texture : register(t0);
sampler Sampler : register(s0);

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

float4 main( VS_OUTPUT inputData) : SV_TARGET
{
    return activeColor * Texture.Sample( Sampler, inputData.texcoord );
}
