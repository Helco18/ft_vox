#pragma once

#include <cstdint>
#include <unordered_map>
#include <string>
#include <vector>
#include "Pipeline.hpp"

class AEngine;

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
		static Pipeline		getPipeline(PipelineType pipelineType);

	private:
		typedef std::unordered_map<PipelineType, Pipeline> PipelineMap;

		static void			_uploadPipeline(AEngine * engine, PipelineInfo & pipelineInfo, PipelineType pipelineType);
		static PipelineMap	_pipelineMap;

};
