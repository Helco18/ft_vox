#pragma once

#include "AEngine.hpp"

enum PipelineType
{
	PIPELINE_VOXEL,
	PIPELINE_WIREFRAME
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
