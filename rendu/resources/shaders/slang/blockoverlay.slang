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

struct TargetedBlock
{
	float4x4 value;
};

[[vk::binding(0,0)]]
ConstantBuffer<CameraMatrix> cameraMatrix;

[[vk::binding(3,0)]]
ConstantBuffer<TargetedBlock> targetedBlock;

[shader("vertex")]
VertexOutput vertMain(VertexInput input)
{
	VertexOutput output;

	output.sv_position = mul(cameraMatrix.proj, mul(cameraMatrix.view, mul(targetedBlock.value, float4(input.position, 1.0))));
	return output;
}

[shader("fragment")]
float4 fragMain(VertexOutput output) : SV_Target
{
	return float4(1.0f, 1.0f, 1.0f, 0.25f);
}
