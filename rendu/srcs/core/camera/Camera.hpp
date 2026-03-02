#pragma once

#include "AEngine.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

#define CAMERA_SPEED 1.0f
#define NEAR_PLANE_OFFSET 0.5f

enum CameraType
{
	EULER,
	SIX_DOF,
	FPS
};

enum FrustumDir
{
	FRUSTUM_LEFT,
	FRUSTUM_RIGHT,
	FRUSTUM_BOTTOM,
	FRUSTUM_TOP
};

struct Plane
{
	float	plane[4];
};

struct CameraBuffer
{
	glm::mat4 view;
	glm::mat4 proj;
	glm::vec3 camPos;
};

class Camera
{
	public:
		Camera(glm::vec3 position, int width, int height);

		const glm::vec3 &			getPosition() const { return _position; }
		const glm::quat &			getOrientation() const { return _orientation; }
		const glm::vec3	&			getAltitude() const { return _altitude; }
		CameraType					getCameraType() const { return _type; }
		int							getWidth() const { return _width; }
		int							getHeight() const { return _height; }
		float						getYaw() const { return _yaw; }
		float						getPitch() const { return _pitch; }
		float						getFOV() const { return _fov; }
		float						getSensitivity() const { return _sensitivity; }
		float						getSpeed() const { return _speed; }
		uint8_t						getRenderDistance() const { return _renderDistance; }
		const CameraBuffer &		getBuffer() const { return _cameraBuffer; }
		const glm::mat4 &			getView() const { return _view; }
		const glm::mat4 &			getProj() const { return _cameraBuffer.proj; }
		const Plane * 				getPlanes() const { return _planes; }

		void						setPosition(const glm::vec3 & position) { _position = position; }
		void						setOrientation(glm::vec3 orientation) { _orientation = orientation; }
		void						setYaw(float yaw) { _yaw = yaw; }
		void						setPitch(float pitch) { _pitch = pitch; }
		void						setFOV(float fov) { _fov = fov; }
		void						setWidth(int width) { _width = width; }
		void						setHeight(int height) { _height = height; }
		void						setSpeed(float speed) { _speed = speed; }
		void						setRenderDistance(int renderDistance) { _renderDistance = renderDistance; }
		void						setCameraType(CameraType type);

		void						changeYaw(float yaw) { _yaw += yaw; }
		void						changePitch(float pitch) { _pitch += pitch; }
		void						changeSpeed(float speed) { _speed += speed; }

		glm::vec3					computeForward(bool ignoreY) const;
		glm::vec3					computeRight() const;
		glm::vec3					computeUp() const;
		glm::mat4					computeView() const;

		void						addPipelineToRender(PipelineType pipelineType);
		void						removePipelineToRender(PipelineType pipelineType);

		void						renderViewMatrix(AEngine * engine, bool resized = false);
		void						updateOrientation(double mouseX, double mouseY, float roll);

		glm::vec3					getEulerAngles() const;
	private:
		void						_extractPlanes();

		glm::vec3					_position;
		glm::quat					_orientation;
		glm::vec3					_altitude;
		uint8_t						_renderDistance = 2;
		int							_width;
		int							_height;
		float						_yaw;
		float						_pitch;
		float						_fov;
		float						_sensitivity;
		float						_speed;
		std::vector<PipelineType>	_pipelines;
		CameraType					_type = EULER;
		CameraBuffer				_cameraBuffer;
		glm::mat4					_view;
		Plane						_planes[4];
		float						_aspectRatio = 0.0f;
};
