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
		typedef std::unordered_map<PipelineType, Pipeline> PipelineCache;

		PipelineManager() = delete;
		~PipelineManager() = delete;

		static void						init(AEngine * engine);
		static Pipeline					getPipeline(PipelineType pipelineType);
		static const PipelineCache &	getAllPipelines() { return _pipelineCache; }

	private:
		static void						_uploadPipeline(AEngine * engine, PipelineInfo & pipelineInfo, PipelineType pipelineType);
		static PipelineCache			_pipelineCache;

};
