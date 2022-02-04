struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

Texture2D Texture : register(t0);
sampler Sampler : register(s0);

float4 main(VS_OUTPUT input) : SV_TARGET
{
	float4 color = Texture.Sample( Sampler, input.uv );
	return color;
}