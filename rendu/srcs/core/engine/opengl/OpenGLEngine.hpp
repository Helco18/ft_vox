#include "AEngine.hpp"
#include <string>

class OpenGLEngine : public AEngine
{
	public:
		OpenGLEngine(GLFWwindow * window, Camera * camera);
		~OpenGLEngine();

		void		load() override;
		void		beginFrame() override;
		AssetID		uploadAsset(Asset & asset) override;
		PipelineID	uploadPipeline(PipelineInfo & pipelineInfo) override;
		void		drawAsset(AssetID assetID, PipelineID pipelineID) override;
		void		endFrame() override;

	private:
		typedef std::unordered_map<PipelineID, PipelineInfo> PipelineMap;

		PipelineMap	_pipelineMap;

		GLuint		_ubo;
		GLuint		_texture;
		GLuint		_shader;

		void		_createShader(const std::string & vertexPath, const std::string & fragmentPath);
		void		_updateUniformBuffer();
		void		_handleResize();
		void		_createTexture();
};
