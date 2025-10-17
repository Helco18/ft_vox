#include "Camera.hpp"

Camera::Camera(glm::vec3 position): _position(position)
{
	_altitude = glm::vec3(0.0f, 1.0f, 0.0f);
	_yaw = 90.f;
	_pitch = 0.f;
	_FOV = 90;
}

glm::vec3 Camera::getPosition() const
{
	return _position;
}

glm::vec3 Camera::getAltitude() const
{
	return _altitude;
}

float Camera::getYaw() const
{
	return _yaw;
}

float Camera::getPitch() const
{
	return _pitch;
}

float Camera::getFOV() const
{
	return _FOV;
}

void Camera::setPosition(const glm::vec3 & position)
{
	_position = position;
}
