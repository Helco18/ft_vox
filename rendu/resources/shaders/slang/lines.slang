struct VertexInput
{
	float3 position;
}

struct VertexOutput
{
    float4 sv_position : SV_Position;
    float3 outPosition;
    float4 worldPos;
}

struct CameraMatrix
{
	float4x4 view;
    float4x4 proj;
};

struct LineData
{
    float4x4 model;
};

[[vk::binding(0,0)]]
ConstantBuffer<CameraMatrix> cameraMatrix;

#ifdef __VULKAN__
[[push_constant]]
#else
[[vk::binding(1, 0)]]
#endif
ConstantBuffer<LineData> lineData;

[shader("vertex")]
VertexOutput vertMain(VertexInput input)
{
	VertexOutput output;

    output.outPosition = input.position;
    float4 worldPos = mul(lineData.model, float4(input.position, 1.0));
    output.sv_position = mul(cameraMatrix.proj, mul(cameraMatrix.view, worldPos));
    output.worldPos = worldPos;
	return output;
}

[shader("fragment")]
float4 fragMain(VertexOutput output) : SV_Target
{
    return float4((abs(output.worldPos.x) % 255) / 255, (abs(output.worldPos.y) % 255) / 255, (abs(output.worldPos.z) % 255) / 255, 1.0f);
}
