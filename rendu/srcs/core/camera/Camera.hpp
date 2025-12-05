#pragma once

#include "glm/glm.hpp"

#define CAMERA_SPEED 2.0f

class Camera
{
	public:
		Camera(glm::vec3 position, int width, int height);

		glm::vec3	getPosition() const { return _position; }
		glm::vec3	getOrientation() const { return _orientation; }
		glm::vec3	getAltitude() const { return _altitude; }
		int			getWidth() const { return _width; }
		int			getHeight() const { return _height; }
		float		getYaw() const { return _yaw; }
		float		getPitch() const { return _pitch; }
		float		getFOV() const { return _FOV; }
		float		getSensitivity() const { return _sensitivity; }
		float		getSpeed() const { return _speed; }
		int			getRenderDistance() const {return _renderDistance; }

		void		setPosition(const glm::vec3 & position) { _position = position; }
		void		setOrientation(glm::vec3 orientation) { _orientation = orientation; }
		void		setYaw(float yaw) { _yaw = yaw; }
		void		setPitch(float pitch) { _pitch = pitch; }
		void		setWidth(int width) { _width = width; }
		void		setHeight(int height) { _height = height; }
		void		setSpeed(float speed) { _speed = speed; }
		void		setRenderDistance(int renderDistance) { _renderDistance = renderDistance; }

		void		changeYaw(float yaw) { _yaw += yaw; }
		void		changePitch(float pitch) { _pitch += pitch; }
		void		changeSpeed(float speed) { _speed += speed; }

		glm::vec3	computeForward() const;

		void		updateOrientation(double mouseX, double mouseY);
	private:
		glm::vec3	_position;
		glm::vec3	_orientation;
		glm::vec3	_altitude;
		int			_renderDistance = 6;
		int			_width;
		int			_height;
		float		_yaw;
		float		_pitch;
		int			_FOV;
		float		_sensitivity;
		float		_speed;
};
