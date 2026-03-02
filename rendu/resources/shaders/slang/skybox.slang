struct VertexInput
{
    float3 position;
    float3 normal;
};

struct VertexOutput
{
    float4 sv_position : SV_Position;
    float3 outNormal;
    float3 camPos;
    float dirY;
};

struct CameraMatrix
{
    float4x4 view;
    float4x4 proj;
    float3 camPos;
};

[[vk::binding(0, 0)]]
ConstantBuffer<CameraMatrix> cameraMatrix;

[shader("vertex")]
VertexOutput vertMain(VertexInput input)
{
    VertexOutput output;
    float4x4 viewNoT = cameraMatrix.view;
    viewNoT[0][3] = float(0.0f);
    viewNoT[1][3] = float(0.0f);
    viewNoT[2][3] = float(0.0f);
    viewNoT[3][3] = float(1.0f);
    output.sv_position = mul(cameraMatrix.proj, mul(viewNoT, float4(input.position, 1.0)));
	output.sv_position.z = output.sv_position.w;
    output.outNormal = input.normal;
    output.dirY = clamp((normalize(input.position).y + 1.0) * 0.5, 0.0, 1.0);
    output.camPos = cameraMatrix.camPos;
    return output;
}

[shader("fragment")]
float4 fragMain(VertexOutput output) : SV_Target
{
    float4 color;

    if (output.camPos.y <= -480.0f)
        color = float4(35.0f / 255.0f, 0.0f, 0.0f, 1.0f);
    else
    {
        float3 horizonColor = float3(1.0f, 1.0f, 1.0f);
        float3 zenithColor = float3(102.0f / 255.0f, 154.0f / 255.0f, 1.0f);
        color = float4(lerp(horizonColor, zenithColor, output.dirY), 1.0f);
    }
    return color;
}
