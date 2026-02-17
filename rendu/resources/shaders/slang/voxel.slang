struct VertexInput
{
	float3 inPosition;
	float3 inNormal;
	float inAlpha;
	float2 inTexCoord;
	float2 inUvMin;
	float2 inUvMax;
	float2 inUvRepeat;
}

struct VertexOutput
{
	float4 sv_position : SV_Position;
	float3 outNormal;
	float outAlpha;
	float2 outTexCoord;
	float2 outUvMin;
	float2 outUvMax;
	float2 outUvRepeat;
	float3 worldPos;
	float3 camPos;
};

struct CameraMatrix
{
	float4x4 view;
	float4x4 proj;
	float3 camPos;
};

struct ChunkData
{
	float fadeValue;
	float maxDistanceRendered;
	float time;
};

[[vk::binding(0,0)]]
ConstantBuffer<CameraMatrix> cameraMatrix;

[[vk::binding(1,0)]]
Sampler2D texture;

#ifdef __VULKAN__
[[push_constant]]
#else
[[vk::binding(2,0)]]
#endif
ConstantBuffer<ChunkData> chunkData;

[shader("vertex")]
VertexOutput vertMain(VertexInput input)
{
	VertexOutput output;
	// x y z w (w étant la composante pour passer du clip space au normalized device coordinates, gère la perspective/profondeur)
	output.sv_position = mul(cameraMatrix.proj, mul(cameraMatrix.view, float4(input.inPosition, 1.0)));
	output.outNormal = input.inNormal;
	output.outTexCoord = input.inTexCoord;
	output.worldPos = input.inPosition;
	output.outUvMin = input.inUvMin;
	output.outUvMax = input.inUvMax;
	output.outUvRepeat = input.inUvRepeat;
	output.outAlpha = input.inAlpha;
	output.camPos = cameraMatrix.camPos;
	return output;
}

[shader("fragment")]
float4 fragMain(VertexOutput output) : SV_Target
{
	// light -----------------------------------------------------------------
	float3 lightDir = normalize(float3(-0.5, 1.0, -0.5));
	float diffuse = max(dot(output.outNormal, lightDir), 0.2);
	float3 ambientColor = float3(0.2, 0.2, 0.2);
	diffuse = diffuse + ambientColor.r;

	// UV + texture ------------------------------------------------------------
	float2 tileSize = output.outUvMax - output.outUvMin;
	// frac(x) retourne la partie décimale de x (equivalent a x - floor(x))
	float2 finalUV = output.outUvMin + frac(output.outTexCoord * output.outUvRepeat) * tileSize;
	// Samle return un float4 (RGBA) de la texture à l'UV donné
	float4 vertColor = texture.Sample(finalUV) * float4(1.0f, 1.0f, 1.0f, chunkData.fadeValue * output.outAlpha);
	vertColor.rgb *= diffuse;

	// Fog ---------------------------------------------------------------------
	float3 fogColor = float3(0.7, 0.8, 1.0);
	float fogStart = chunkData.maxDistanceRendered * 0.5f;
	float fogEnd = chunkData.maxDistanceRendered;
	float distanceToCamera = length(output.worldPos - output.camPos);
	// saturate(x) clamp x entre 0 et 1
	float fogFactor = saturate((fogEnd - distanceToCamera) / (fogEnd - fogStart));

	float3 finalColor = lerp(fogColor, vertColor.rgb, fogFactor);

	return float4(finalColor, vertColor.a);
}
