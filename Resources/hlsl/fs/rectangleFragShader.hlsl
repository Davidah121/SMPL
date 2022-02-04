struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

float4 main( VS_OUTPUT inputData) : SV_TARGET
{
    return inputData.color;
}
