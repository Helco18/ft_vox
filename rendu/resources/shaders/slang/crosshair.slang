struct VertexInput
{
	float2 position;
	float2 uv;
}

struct VertexOutput
{
	float4 sv_position : SV_Position;
	float2 outUv;
}

struct AspectRatio
{
	float4x4 proj;
	float4x4 model;
};

[[vk::binding(1,0)]]
ConstantBuffer<AspectRatio> aspectRatio;

[[vk::binding(2,0)]]
Sampler2D texture;

[shader("vertex")]
VertexOutput vertMain(VertexInput input)
{
	VertexOutput output;
	float4 position;

	position = float4(input.position, 0.0f, 1.0f);
	output.sv_position = mul(aspectRatio.proj, mul(aspectRatio.model, position));
	output.outUv = input.uv;
	return output;
}

[shader("fragment")]
float4 fragMain(VertexOutput output) : SV_Target
{
	float4 texColor = texture.Sample(output.outUv);
	if (texColor.a == 0)
		discard;
	return texColor;
}
