struct VertexInput
{
	float2 position;
}

struct VertexOutput
{
	float4 sv_position : SV_Position;
}

[shader("vertex")]
VertexOutput vertMain(VertexInput input)
{
	VertexOutput output;

	output.sv_position = float4(input.position, 1.0f, 1.0f);
	return output;
}

[shader("fragment")]
float4 fragMain() : SV_Target
{
	return float4(0.4f, 0.0f, 0.3f, 1.0f);
}
