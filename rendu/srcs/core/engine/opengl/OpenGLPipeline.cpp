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
		else if (descriptorInfo.type == DescriptorType::COMBINED_IMAGE_SAMPLER)
		{
			TextureBuffer textureBuffer;
			textureBuffer.name = descriptorInfo.name;
			_createTexture(textureBuffer, descriptorInfo.textureInfo);
			pipelineLayout.textureBuffers.try_emplace(descriptorInfo.binding, textureBuffer);
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

PipelineLayout & OpenGLEngine::_applyPipeline(PipelineID pipelineID)
{
	PipelineMap::iterator pipelineit = _pipelineMap.find(pipelineID);
	if (pipelineit == _pipelineMap.end())
		throw OpenGLException("Requested unknown pipeline with ID: " + toString(pipelineID));

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

	int i = 0;
	for (std::pair<const unsigned int, TextureBuffer> & texturePair : pipelineLayout.textureBuffers)
	{
		if (i >= 31)
			throw OpenGLException("Texture slots overflow.");
		TextureBuffer & textureBuffer = texturePair.second;
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textureBuffer.tbo);
		GLuint textureIndex = glGetUniformLocation(shaderit->second, textureBuffer.name.c_str());
		if (textureIndex == GL_INVALID_INDEX)
			throw OpenGLException("Couldn't find texture uniform: " + textureBuffer.name);
		glUniform1i(textureIndex, 0);
		i++;
	}
	return pipelineLayout;
}
