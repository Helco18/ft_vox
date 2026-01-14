#include "AEngine.hpp"
#include <GL/glew.h>
#include "GLFW/glfw3.h"
#include <string>

#define OPENGL_SHADER_PATH "srcs/core/shaders/glsl/"

struct GLValueConverter
{
	static constexpr GLenum getType(AttributeType type)
	{
		switch (static_cast<int>(type))
		{
			case FLOAT3:
			case FLOAT2: return GL_FLOAT;
			case INT: return GL_INT;
		}
		return 0;
	}
};

class OpenGLEngine : public AEngine
{
	public:
		OpenGLEngine(GLFWwindow * window, Camera * camera);
		~OpenGLEngine();

		void		load() override;
		void		beginFrame() override;
		AssetID		uploadAsset(Asset & asset, PipelineID pipelineID) override;
		void		unloadAsset(AssetID assetID) override;
		PipelineID	uploadPipeline(PipelineInfo & pipelineInfo) override;
		void		drawAsset(AssetID assetID, PipelineID pipelineID) override;
		void		endFrame() override;

	private:
		typedef std::unordered_map<PipelineID, PipelineInfo>	PipelineMap;
		typedef std::unordered_map<std::string, GLuint>			ShaderCache;

		PipelineMap	_pipelineMap;
		ShaderCache	_shaderCache;

		GLuint		_ubo;
		GLuint		_texture;

		static void	_debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei, const GLchar *message, const void *);
		GLuint		_createShader(const std::string & vertexPath, const std::string & fragmentPath);
		void		_updateUniformBuffer();
		void		_handleResize();
		void		_createTexture();
		void		_applyPipeline(PipelineID pipelineID);
};
