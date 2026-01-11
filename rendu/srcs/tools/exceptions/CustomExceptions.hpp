#pragma once

#include "Logger.hpp"
#include <exception>
#include <string>

class CustomException: public std::exception
{
	public:
		explicit CustomException(LogSource source, const std::string & message): _source(source), _message(message) {}
		const char *	what() const noexcept override { return _message.c_str(); };
		LogSource		getSource() const { return _source; }

	private:
		LogSource		_source;
		std::string		_message;
};

class ModelException: public CustomException { public: ModelException(const std::string & message): CustomException(MODEL, message) {} };
class OpenGLException: public CustomException { public: OpenGLException(const std::string & message): CustomException(ENGINE_OPENGL, message) {} };
class VulkanException: public CustomException { public: VulkanException(const std::string & message): CustomException(ENGINE_VULKAN, message) {} };
class WindowException: public CustomException { public: WindowException(const std::string & message): CustomException(WINDOW, message) {} };
class EnvironmentException: public CustomException { public: EnvironmentException(const std::string & message): CustomException(ENVIRONMENT, message) {} };
class GeneralException: public CustomException { public: GeneralException(const std::string & message): CustomException(GENERAL, message) {} };
class VoxelException: public CustomException { public: VoxelException(const std::string & message): CustomException(VOXEL, message) {} };
class TextureException: public CustomException { public: TextureException(const std::string & message): CustomException(TEXTURE, message) {} };
class PipelineException: public CustomException { public: PipelineException(const std::string & message): CustomException(PIPELINE, message) {} };
class ThreadException: public CustomException { public: ThreadException(const std::string & message): CustomException(THREAD, message) {} };
