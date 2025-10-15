struct VertexInput
{
    float2 inPosition;
    float3 inColor;
}

struct VertexOutput
{
    float4 sv_position : SV_Position;
	float3 outColor;
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
    output.sv_position = mul(ubo.proj, mul(ubo.view, mul(ubo.model, float4(input.inPosition, 0.0, 1.0))));
    output.outColor = input.inColor;
    return output;
}

[shader("fragment")]
float4 fragMain(VertexOutput inVert) : SV_Target
{
    return float4(inVert.outColor, 1.0);
}
