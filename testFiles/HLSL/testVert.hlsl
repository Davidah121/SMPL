struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

VS_OUTPUT main( float2 pos : POSITION, float2 textureCoords : COLOR )
{
	VS_OUTPUT output;
	output.position = float4(pos, 0.0, 1.0);
	output.uv = textureCoords;

	return output;
}