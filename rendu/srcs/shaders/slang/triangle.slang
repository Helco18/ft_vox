struct VertexInput
{
	float3 inPosition;
	float3 inColor;
	float2 inTexCoord;
}

struct VertexOutput
{
	float4 sv_position : SV_Position;
	float3 outColor;
	float2 fragTexCoord;
};

struct UniformBuffer
{
	float4x4 model;
	float4x4 view;
	float4x4 proj;
};

ConstantBuffer<UniformBuffer> ubo;

[shader("vertex")]
VertexOutput vertMain(VertexInput input)
{
	VertexOutput output;
	// x y z w (w étant la composante pour passer du clip space au normalized device coordinates, gère la perspective/profondeur)
	output.sv_position = mul(ubo.proj, mul(ubo.view, mul(ubo.model, float4(input.inPosition, 1.0))));
	output.outColor = input.inColor;
	output.fragTexCoord = input.inTexCoord;
	return output;
}

Sampler2D texture;

[shader("fragment")]
float4 fragMain(VertexOutput inVert) : SV_Target
{
	return float4(inVert.outColor * texture.Sample(inVert.fragTexCoord).rgb, 1.0);
}
