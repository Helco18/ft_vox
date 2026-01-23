struct VertexInput
{
	float3 position;
}

struct VertexOutput
{
	float4 sv_position : SV_Position;
}

struct CameraMatrix
{
	float4x4 view;
	float4x4 proj;
};

[[vk::binding(0,0)]]
ConstantBuffer<CameraMatrix> cameraMatrix;

[shader("vertex")]
VertexOutput vertMain(VertexInput input)
{
	VertexOutput output;

	output.sv_position = mul(cameraMatrix.proj, mul(cameraMatrix.view, float4(input.position, 1.0)));
	return output;
}

[shader("fragment")]
float4 fragMain() : SV_Target
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
