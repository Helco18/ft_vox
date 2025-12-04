#include "AEngine.hpp"
#include <string>

class OpenGLEngine : public AEngine
{
	public:
		OpenGLEngine(GLFWwindow * window, Camera * camera, bool isWireframeEnabled);
		~OpenGLEngine();

		void	load() override;
		void	beginFrame() override;
		AssetID	uploadAsset(Asset & asset) override;
		void	drawAsset(AssetID assetID) override;
		void	endFrame() override;

	private:
		GLuint	_ubo;
		GLuint	_texture;
		GLuint	_shader;

		void	_createShader(const std::string & vertexPath, const std::string & fragmentPath);
		void	_updateUniformBuffer();
		void	_handleResize();
		void	_createTexture();
};
