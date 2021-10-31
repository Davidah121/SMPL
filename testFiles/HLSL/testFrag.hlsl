cbuffer UniformData : register(b0)
{
	float4 r;
};

float4 main() : SV_TARGET
{
	return r;
}