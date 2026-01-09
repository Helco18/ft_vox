struct VertexInput
{
	float3 position;
}

struct VertexOutput
{
	float4 sv_position : SV_Position;
}

[shader("vertex")]
VertexOutput vertMain(VertexInput input)
{
	VertexOutput output;

	output.sv_position = float4(input.position, 1.0f);
	return output;
}

[shader("fragment")]
float4 fragMain() : SV_Target
{
	return float4(1.0f);
}
