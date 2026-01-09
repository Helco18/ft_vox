#pragma once

#include <unordered_map>
#include <string>
#include <vector>

class AEngine;

typedef unsigned int PipelineID;

enum AttributeType
{
	FLOAT = 0x1406,
	INT
};

enum PolygonMode
{
	FILL,
	LINE
};

enum CullMode
{
	BACK,
	OFF
};

enum PipelineType
{
	PIPELINE_VOXEL,
	PIPELINE_WIREFRAME
};

struct Attribute
{
	size_t 			size;
	int				count;
	AttributeType	type;
	bool			normalized;
};

struct PipelineInfo
{
	std::string				shaderName;
	PolygonMode				polygonMode = FILL;
	CullMode				cullMode = BACK;
	int						width = -1;
	int						height = -1;
	bool					blend = true;
	bool					depthTest = true;
	std::vector<Attribute>	attributes;
};

class PipelineManager
{
	public:
		PipelineManager() = delete;
		~PipelineManager() = delete;

		static void			init(AEngine * engine);
		static PipelineID	getPipeline(PipelineType pipelineType);

	private:
		typedef std::unordered_map<PipelineType, PipelineID> PipelineMap;

		static void			_uploadPipeline(AEngine * engine, PipelineInfo & pipelineInfo, PipelineType pipelineType);
		static PipelineMap	_pipelineMap;

};
