#include "OpenGLEngine.hpp"

OpenGLEngine::OpenGLEngine(GLFWwindow * window, Camera * camera) : AEngine(window, camera) {}

OpenGLEngine::~OpenGLEngine() {}

void OpenGLEngine::load()
{
	glfwSwapInterval(0);
	OBJModel model = OBJModel::getModel(CUBE);

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, model.getVertices().size() * sizeof(Vertex), model.getVertices().data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texCoord));

	glGenBuffers(1, &_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.getIndices().size() * sizeof(uint32_t), model.getIndices().data(), GL_STATIC_DRAW);

	_createShader("triangle.vert", "triangle.frag");
	glUseProgram(_shader);

	glGenBuffers(1, &_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, _ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformBuffer), nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, _ubo);

	_createTexture();

	_indexSize = model.getIndices().size();

	glBindVertexArray(0);

	std::cout << GREEN << "[OK] OpenGL engine initialized successfully." << RESET << std::endl;
	_isInitalized = true;
}

void OpenGLEngine::drawFrame()
{
	if (_isFramebufferResized)
		_handleResize();

	glfwSwapBuffers(_window);
	glBindVertexArray(_vao);
	_updateUniformBuffer();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texture);
	GLuint textureIndex = glGetUniformLocation(_shader, "texture_0");
	if (textureIndex == GL_INVALID_INDEX)
		throw std::runtime_error("Couldn't find texture uniform.");
	glUniform1i(textureIndex, 0);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawElements(GL_TRIANGLES, _indexSize, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void OpenGLEngine::_handleResize()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, _camera->getWidth(), _camera->getHeight());
	_isFramebufferResized = false;
}
