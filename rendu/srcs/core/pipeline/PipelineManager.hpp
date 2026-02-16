#pragma once

#include <cstdint>
#include <unordered_map>
#include <string>
#include <vector>

class AEngine;

typedef unsigned int PipelineID;

enum PipelineType
{
	PIPELINE_VOXEL,
	PIPELINE_WIREFRAME,
	PIPELINE_BASIC,
	PIPELINE_SKYBOX,
	PIPELINE_LINES,
	PIPELINE_CROSSHAIR
};

enum AttributeType
{
	FLOAT,
	FLOAT3,
	FLOAT2,
	INT
};

enum TextureFiltering
{
	LINEAR,
	NEAREST
};

enum PolygonMode
{
	FILL,
	LINE
};

enum DrawMode
{
	TRIANGLES,
	LINES
};

enum CullMode
{
	BACK,
	OFF
};

enum DescriptorType
{
	UNIFORM_BUFFER,
	COMBINED_IMAGE_SAMPLER,
	PUSH_CONSTANT
};

enum ShaderStage
{
	VERTEX,
	FRAGMENT,
	ALL
};

struct Attribute
{
	size_t 			size;
	int				count;
	AttributeType	type;
	bool			normalized;
};

struct TextureInfo
{
	int					width;
	int					height;
	int					colorChannels;
	void *				data;
	TextureFiltering	filtering = TextureFiltering::LINEAR;
};

struct DescriptorInfo
{
	std::string		name;
	uint32_t		binding;
	DescriptorType	type;
	uint32_t		count;
	ShaderStage		stage;
	uint32_t		size;
	TextureInfo		textureInfo; // Exists only if type has texture
};

struct UniformInfo
{
	size_t	size;
	void *	data;
};

struct PipelineInfo
{
	PipelineID					id;
	std::string					shaderName;
	DrawMode					drawMode = TRIANGLES;
	PolygonMode					polygonMode = FILL;
	CullMode					cullMode = BACK;
	int							width = -1;
	int							height = -1;
	bool						blend = true;
	bool						depthTest = true;
	std::vector<Attribute>		attributes;
	uint32_t					attributeSize = 0;
	std::vector<DescriptorInfo>	descriptors; // For uniform buffers
	size_t						uniformSize = 0; // For small uniforms/push constants
};

class PipelineManager
{
	public:
		PipelineManager() = delete;
		~PipelineManager() = delete;

		static void					init(AEngine * engine);
		static const PipelineInfo &	getPipeline(PipelineType pipelineType);
		static void					uploadPipeline(AEngine * engine, PipelineInfo & pipelineInfo, PipelineType pipelineType);

	private:
		typedef std::unordered_map<PipelineType, PipelineInfo> PipelineMap;

		static PipelineMap			_pipelineMap;

};
