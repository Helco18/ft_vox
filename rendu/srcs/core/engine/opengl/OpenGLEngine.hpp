#include "AEngine.hpp"
#include <string>

class OpenGLEngine : public AEngine
{
	public:
		OpenGLEngine(GLFWwindow * window, Camera * camera);
		~OpenGLEngine();

		void	load() override;
		AssetID	upload(Asset & asset) override;
		void	drawAsset(AssetID asset) override;

	private:
		GLuint	_ubo;
		GLuint	_texture;
		size_t	_indexSize;
		GLuint	_shader;

		void	_createShader(const std::string & vertexPath, const std::string & fragmentPath);
		void	_updateUniformBuffer();
		void	_handleResize();
		void	_createTexture();
};
