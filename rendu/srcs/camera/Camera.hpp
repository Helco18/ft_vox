#pragma once

#include "glm/glm.hpp"

#define CAMERA_SPEED 5.0f

class Camera
{
	public:
		Camera(glm::vec3 position);

		glm::vec3	getPosition() const;
		glm::vec3	getAltitude() const;
		float		getYaw() const;
		float		getPitch() const;
		float		getFOV() const;

		void		setPosition(const glm::vec3 & position);
	private:
		glm::vec3	_position;
		glm::vec3	_altitude;
		float		_yaw;
		float		_pitch;
		int			_FOV;
};
