#include "Camera.hpp"

Camera::Camera(glm::vec3 position, int width, int height): _position(position), _width(width), _height(height)
{
	_altitude = glm::vec3(0.0f, 1.0f, 0.0f);
	_yaw = -90.0f;
	_pitch = 0.0f;
	_FOV = 90;
	_sensitivity = 200.0f;
	_speed = CAMERA_SPEED;

	glm::vec3 direction;
	direction.x = cosf(glm::radians(_yaw)) * cosf(glm::radians(_pitch));
	direction.y = sinf(glm::radians(_pitch));
	direction.z = sinf(glm::radians(_yaw)) * cosf(glm::radians(_pitch));
	_orientation = glm::normalize(direction);
}

static glm::vec3 translateDirection(const float yaw, const float pitch)
{
	glm::vec3 direction;
	direction.x = cosf(glm::radians(yaw)) * cosf(glm::radians(pitch));
	direction.y = sinf(glm::radians(pitch));
	direction.z = sinf(glm::radians(yaw)) * cosf(glm::radians(pitch));
	return (direction);
}

void Camera::updateOrientation(double mouseX, double mouseY)
{
	const float rotX = _sensitivity * static_cast<float>(mouseY - (static_cast<float>(_height) / 2)) / static_cast<float>(_height);
	const float rotY = _sensitivity * static_cast<float>(mouseX - (static_cast<float>(_width) / 2)) / static_cast<float>(_width);

	_yaw += rotY;
	_pitch -= rotX;

	if (_pitch> 89.98f)
		_pitch = 89.98f;
	else if (_pitch< -89.98f)
		_pitch = -89.98f;

	if (_yaw > 180.0f)
		_yaw -= 360.0f;
	else if (_yaw < -180.0f)
		_yaw += 360.0f;

	setOrientation(translateDirection(_yaw, _pitch));
}

glm::vec3 Camera::computeForward() const
{
	const float radYaw = glm::radians(_yaw);
	const float radPitch = glm::radians(_pitch);

	glm::vec3 forward;
	forward.x = cosf(radPitch) * sinf(radYaw);
	forward.y = sinf(radPitch);
	forward.z = -cosf(radPitch) * cosf(radYaw);
	forward = glm::normalize(forward);
	
	return forward;
}
