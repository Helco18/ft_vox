struct VertexInput
{
	float3 position;
}

struct VertexOutput
{
    float4 sv_position : SV_Position;
    float3 outPosition;
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

    output.outPosition = input.position;
	output.sv_position = mul(cameraMatrix.proj, mul(cameraMatrix.view, float4(input.position, 1.0)));
	return output;
}

[shader("fragment")]
float4 fragMain(VertexOutput xd) : SV_Target
{
    return float4((abs(xd.outPosition.x) % 255) / 255, (abs(xd.outPosition.y) % 255) / 255, (abs(xd.outPosition.z) % 255) / 255, 1.0f);
    // return float4(0.2f, 0.0f, 0.7f, 1.0f);
}
