#include "AEngine.hpp"

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
		GLuint	_tbo;
		size_t	_indexSixe;
		GLuint	_shader;

		void	_createShader(const std::string & vertexPath, const std::string & fragmentPath);
};
