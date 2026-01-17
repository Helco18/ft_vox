#pragma once

#include <string>
#include <vector>

typedef unsigned int PipelineID;

enum AttributeType
{
	FLOAT3,
	FLOAT2,
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

struct Attribute
{
	size_t 			size;
	int				count;
	AttributeType	type;
	bool			normalized;
};

struct PipelineInfo
{
	PipelineID				id;
	std::string				shaderName;
	PolygonMode				polygonMode = FILL;
	CullMode				cullMode = BACK;
	int						width = -1;
	int						height = -1;
	bool					blend = true;
	bool					depthTest = true;
	std::vector<Attribute>	attributes;
	uint32_t				attributeSize = 0;
};

class Pipeline
{
	public:
		Pipeline(PipelineInfo & pipelineInfo): _pipelineInfo(pipelineInfo) {};
		~Pipeline() {};

		const PipelineInfo &	getPipelineInfo() const { return _pipelineInfo; };

	private:
		PipelineInfo	_pipelineInfo;
};
