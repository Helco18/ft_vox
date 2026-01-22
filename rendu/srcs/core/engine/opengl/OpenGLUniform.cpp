#include "OpenGLEngine.hpp"
#include "Logger.hpp"
#include "utils.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>
#include <algorithm>

void OpenGLEngine::updateUniformBuffer(PipelineID pipelineID, unsigned int binding, void * data, size_t size)
{
	PipelineMap::iterator pipelineit = _pipelineMap.find(pipelineID);
	if (pipelineit == _pipelineMap.end())
	{
		Logger::log(ENGINE_OPENGL, WARNING, "Tried to update uniform buffer to unknown Pipeline ID #" + toString(pipelineID));
		return;
	}
	PipelineLayout & pipelineLayout = pipelineit->second;
	std::unordered_map<unsigned int, UniformBufferStream>::iterator it = pipelineLayout.uniformBufferStreams.find(binding);
	if (it == pipelineLayout.uniformBufferStreams.end())
	{
		Logger::log(ENGINE_OPENGL, WARNING, "Tried to update uniform buffer to non-existent binding #" + toString(binding) + ".");
		return;
	}
	UniformBufferStream & uniformBufferStream = it->second;
	glBindBuffer(GL_UNIFORM_BUFFER, uniformBufferStream.ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	return;
}
