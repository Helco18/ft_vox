#include "Camera.hpp"

Camera::Camera(glm::vec3 position, int width, int height): _position(position), _width(width), _height(height)
{
	_altitude = glm::vec3(0.0f, 1.0f, 0.0f);
	_yaw = -90.0f;
	_pitch = 0.0f;
	_fov = 90.0f;
	_sensitivity = 200.0f;
	_speed = CAMERA_SPEED;
	glm::quat pitch = glm::angleAxis(glm::radians(_pitch), glm::vec3(1, 0, 0));
	glm::quat yaw = glm::angleAxis(glm::radians(_yaw), glm::vec3(0, 1, 0));
	glm::quat roll = glm::angleAxis(glm::radians(0.0f), glm::vec3(0, 0, -1));
	_orientation = yaw * roll * pitch;
}

void Camera::updateOrientation(double mouseX, double mouseY, float roll)
{
	const float rotX = -_sensitivity * static_cast<float>(mouseX - (static_cast<float>(_width) / 2)) / static_cast<float>(_width);
	const float rotY = -_sensitivity * static_cast<float>(mouseY - (static_cast<float>(_height) / 2)) / static_cast<float>(_height);

	glm::vec3 up = glm::normalize(_orientation * glm::vec3(0, 1, 0));
	glm::quat qYaw = glm::angleAxis(glm::radians(rotX), up);

	glm::vec3 right = glm::normalize(_orientation * glm::vec3(1, 0, 0));
	glm::quat qPitch = glm::angleAxis(glm::radians(rotY), right);

	glm::vec3 rollD = glm::normalize(_orientation * glm::vec3(0, 0, -1));
	glm::quat qRoll = glm::angleAxis(glm::radians(roll), rollD);
	_orientation = qYaw * qRoll * qPitch * _orientation;

	_orientation = glm::normalize(_orientation);
}

glm::vec3 Camera::computeForward() const
{
	glm::vec3 forward;
	forward = glm::normalize(_orientation * glm::vec3(0.0f, 0.0f, -1.0f));
	
	return forward;
}

glm::mat4 Camera::getView() const
{
	glm::mat4 rot = glm::mat4_cast(glm::conjugate(_orientation));
	glm::mat4 trans = glm::translate(glm::mat4(1.0f), -_position);
	return(rot * trans);
}

glm::vec3 Camera::getEulerAngles() const
{
    glm::vec3 euler = glm::eulerAngles(_orientation);
    euler = glm::degrees(euler);

    return euler;
}
