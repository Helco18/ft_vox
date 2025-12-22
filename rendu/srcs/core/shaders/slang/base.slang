struct VertexInput
{
	float3 inPosition;
    float3 inNormal;
    float2 inTexCoord;
    float2 inUvMin;
    float2 inUvMax;
    float2 inUvRepeat;
}

struct VertexOutput
{
	float4 sv_position : SV_Position;
	float3 outNormal;
    float2 outTexCoord;
    float2 outUvMin;
    float2 outUvMax;
    float2 outUvRepeat;
};

struct UniformBuffer
{
	float4x4 view;
	float4x4 proj;
};

ConstantBuffer<UniformBuffer> ubo;

[shader("vertex")]
VertexOutput vertMain(VertexInput input)
{
	VertexOutput output;
	// x y z w (w étant la composante pour passer du clip space au normalized device coordinates, gère la perspective/profondeur)
	output.sv_position = mul(ubo.proj, mul(ubo.view, float4(input.inPosition, 1.0)));
	output.outNormal = input.inNormal;
	output.outTexCoord = input.inTexCoord;

	output.outUvMin = input.inUvMin;
	output.outUvMax = input.inUvMax;
	output.outUvRepeat = input.inUvRepeat;
	return output;
}

[[vk::binding(1,0)]]
Sampler2D texture;

[shader("fragment")]
float4 fragMain(VertexOutput inVert) : SV_Target
{
    float2 tileSize = inVert.outUvMax - inVert.outUvMin;

	// frac(x) retourne la partie décimale de x (ecivalent a x - floor(x))
	float2 finalUV = inVert.outUvMin + frac(inVert.outTexCoord * inVert.outUvRepeat) * tileSize;

    return texture.Sample(finalUV);
}
