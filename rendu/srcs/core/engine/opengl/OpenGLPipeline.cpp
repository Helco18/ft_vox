#include "OpenGLEngine.hpp"
#include "Logger.hpp"
#include "utils.hpp"
#include "CustomExceptions.hpp"

PipelineID OpenGLEngine::uploadPipeline(PipelineInfo & pipelineInfo)
{
	int pipelineID = _pipelineMap.size();

	PipelineLayout pipelineLayout;
	pipelineLayout.pipelineInfo = pipelineInfo;
	for (DescriptorInfo & descriptorInfo : pipelineLayout.pipelineInfo.descriptors)
	{
		if (descriptorInfo.type == DescriptorType::UNIFORM_BUFFER)
		{
			UniformBufferStream bufferStream;
			bufferStream.binding = descriptorInfo.binding;
			bufferStream.size = descriptorInfo.size;
			bufferStream.data = nullptr;
			glGenBuffers(1, &bufferStream.ubo);
			glBindBuffer(GL_UNIFORM_BUFFER, bufferStream.ubo);
			glBindBufferBase(GL_UNIFORM_BUFFER, descriptorInfo.binding, bufferStream.ubo);
			glBufferData(GL_UNIFORM_BUFFER, descriptorInfo.size, nullptr, GL_DYNAMIC_DRAW);
			pipelineLayout.uniformBufferStreams.try_emplace(descriptorInfo.binding, bufferStream);
		}
	}

	ShaderCache::iterator it = _shaderCache.find(pipelineInfo.shaderName);
	if (it == _shaderCache.end())
	{
		GLuint shader = _createShader(OPENGL_SHADER_PATH + pipelineInfo.shaderName + ".vert",
				OPENGL_SHADER_PATH + pipelineInfo.shaderName + ".frag", pipelineInfo);
		_shaderCache.try_emplace(pipelineInfo.shaderName, shader);
	}
	_pipelineMap.try_emplace(pipelineID, std::move(pipelineLayout));
	return pipelineID;
}

void OpenGLEngine::_applyPipeline(PipelineID pipelineID)
{
	PipelineMap::iterator pipelineit = _pipelineMap.find(pipelineID);
	if (pipelineit == _pipelineMap.end())
	{
		Logger::log(ENGINE_OPENGL, WARNING, "Requested unknown pipeline with ID: " + toString(pipelineID));
		return;
	}

	PipelineLayout & pipelineLayout = pipelineit->second;
	PipelineInfo & pipelineInfo = pipelineLayout.pipelineInfo;

	// Polygon Mode
	switch (static_cast<int>(pipelineInfo.polygonMode))
	{
		case LINE: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
		case FILL: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
	}

	// Cull Mode
	switch (static_cast<int>(pipelineInfo.cullMode))
	{
		case BACK: glEnable(GL_CULL_FACE); glCullFace(GL_BACK); break;
		case OFF: glDisable(GL_CULL_FACE); break;
	}

	pipelineInfo.depthTest ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
	if (pipelineInfo.blend)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
		glDisable(GL_BLEND);

	ShaderCache::iterator shaderit = _shaderCache.find(pipelineInfo.shaderName);
	if (shaderit != _shaderCache.end())
		glUseProgram(shaderit->second);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texture);
	GLuint textureIndex = glGetUniformLocation(shaderit->second, "texture_0");
	if (textureIndex == GL_INVALID_INDEX)
		throw OpenGLException("Couldn't find texture uniform.");
	glUniform1i(textureIndex, 0);
}
