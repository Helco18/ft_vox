#include "OpenGLEngine.hpp"

OpenGLEngine::OpenGLEngine(GLFWwindow * window, Camera * camera) : AEngine(window, camera) {}

OpenGLEngine::~OpenGLEngine() {}

void OpenGLEngine::load()
{
	OBJModel model = OBJModel::getModel(CUBE);

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);
	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, model.getVertices().size() * sizeof(Vertex), model.getVertices().data(),GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &_tbo);
	glBindBuffer(GL_ARRAY_BUFFER, _tbo);
	glBufferData(GL_ARRAY_BUFFER, model.getVertices().size() * sizeof(Vertex), model.getVertices().data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texCoord));
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.getIndices().size() * sizeof(uint32_t), model.getIndices().data(), GL_STATIC_DRAW);

	_indexSixe = model.getIndices().size();

	_createShader("triangle.vert", "triangle.frag");
	glUseProgram(_shader);

	std::cout << GREEN << "[OK] OpenGL engine initialized successfully." << RESET << std::endl;
}

void OpenGLEngine::drawFrame()
{
	UniformBuffer ubo{};

	// Camera orientation (Y-up)
	glm::vec3 camPos = _camera->getPosition();
	float yaw = glm::radians(_camera->getYaw());
	float pitch = glm::radians(_camera->getPitch());

	glm::vec3 forward;
	forward.x = cosf(pitch) * sinf(yaw);
	forward.y = sinf(pitch);
	forward.z = -cosf(pitch) * cosf(yaw);
	forward = glm::normalize(forward);

	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	ubo.view = glm::lookAt(camPos, camPos + forward, up);

	ubo.proj = glm::perspective(glm::radians(_camera->getFOV()),
		(float)_camera->getWidth() / (float)_camera->getHeight(),
		0.01f, 1500.0f);

	GLint viewLoc = glGetUniformLocation(_shader, "block_UniformBuffer_0.view");
	GLint projLoc = glGetUniformLocation(_shader, "block_UniformBuffer_0.proj");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(ubo.view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(ubo.proj));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDrawElements(GL_TRIANGLES, _indexSixe, GL_UNSIGNED_INT, 0);
}
