#include "OpenGLEngine.hpp"
#include "Logger.hpp"
#include "utils.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>

void OpenGLEngine::updateUniformBuffer(PipelineID pipelineID, void * data, size_t size)
{
	PipelineMap::iterator pipelineit = _pipelineMap.find(pipelineID);
	if (pipelineit == _pipelineMap.end())
	{
		Logger::log(ENGINE_OPENGL, WARNING, "Tried to update uniform buffer to unknown Pipeline ID #" + toString(pipelineID));
		return;
	}
	PipelineLayout & pipelineLayout = pipelineit->second;
	glBindBuffer(GL_UNIFORM_BUFFER, pipelineLayout.ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
