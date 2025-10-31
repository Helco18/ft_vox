#include "AEngine.hpp"
#include <string>

class OpenGLEngine : public AEngine
{
	public:
		OpenGLEngine(GLFWwindow * window, Camera * camera);
		~OpenGLEngine();

		void	load() override;
		void	drawFrame() override;

	private:
		GLuint	_vao;
		GLuint	_vbo;
		GLuint	_ibo;
		GLuint	_ubo;
		GLuint	_texture;
		size_t	_indexSize;
		GLuint	_shader;

		void	_createShader(const std::string & vertexPath, const std::string & fragmentPath);
		void	_updateUniformBuffer();
		void	_handleResize();
		void	_createTexture();
};
