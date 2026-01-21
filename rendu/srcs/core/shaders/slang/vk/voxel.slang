struct VertexInput
{
	float3 inPosition;
	float3 inNormal;
	float2 inTexCoord;
	float2 inUvMin;
	float2 inUvMax;
	float2 inUvRepeat;
}

struct VertexOutput
{
	float4 sv_position : SV_Position;
	float3 outNormal;
	float2 outTexCoord;
	float2 outUvMin;
	float2 outUvMax;
	float2 outUvRepeat;
};

struct CameraMatrix
{
	float4x4 view;
	float4x4 proj;
};

struct PushConstant
{
	float	fadeValue;
};

[[vk::binding(0,0)]]
ConstantBuffer<CameraMatrix> camMatrix;

[[vk::binding(1,0)]]
Sampler2D texture;

[[push_constant]]
ConstantBuffer<PushConstant> pushConstant;

[shader("vertex")]
VertexOutput vertMain(VertexInput input)
{
	VertexOutput output;
	// x y z w (w étant la composante pour passer du clip space au normalized device coordinates, gère la perspective/profondeur)
	output.sv_position = mul(camMatrix.proj, mul(camMatrix.view, float4(input.inPosition, 1.0)));
	output.outNormal = input.inNormal;
	output.outTexCoord = input.inTexCoord;

	output.outUvMin = input.inUvMin;
	output.outUvMax = input.inUvMax;
	output.outUvRepeat = input.inUvRepeat;
	return output;
}

[shader("fragment")]
float4 fragMain(VertexOutput inVert) : SV_Target
{
	float3 lightDir = normalize(float3(0.5, 1.0, 0.5));
	float diffuse = max(dot(inVert.outNormal, lightDir), 0.0);

	float3 ambientColor = float3(0.2, 0.2, 0.2);
	diffuse = diffuse + ambientColor.r;

	float2 tileSize = inVert.outUvMax - inVert.outUvMin;
	// frac(x) retourne la partie décimale de x (equivalent a x - floor(x))
	float2 finalUV = inVert.outUvMin + frac(inVert.outTexCoord * inVert.outUvRepeat) * tileSize;
	float4 vertColor = texture.Sample(finalUV) * float4(1.0f, 1.0f, 1.0f, pushConstant.fadeValue);
	// float4 vertColor = texture.Sample(finalUV);

	vertColor.rgb *= diffuse;

	return vertColor;
}
