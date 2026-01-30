#include "Camera.hpp"
#include "Logger.hpp"
#include "utils.hpp"
#include <algorithm>

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
	float rotX = -_sensitivity * static_cast<float>(mouseX - (static_cast<float>(_width) / 2)) / static_cast<float>(_width);
	float rotY = -_sensitivity * static_cast<float>(mouseY - (static_cast<float>(_height) / 2)) / static_cast<float>(_height);

	glm::vec3 up;
	glm::quat qYaw;

	glm::vec3 right;
	glm::quat qPitch;

	glm::vec3 forward;
	glm::quat qRoll;

	_pitch += rotY;
	switch (static_cast<int>(_type))
	{
		case EULER:
		{
			roll = 0;
			up = glm::vec3(0.0f, 1.0f, 0.0f);
			right = glm::vec3(1.0f, 0.0f, 0.0f);
			forward = glm::vec3(0.0f, 0.0f, -1.0f);
			qRoll = glm::angleAxis(glm::radians(0.0f), forward);
			if (_pitch > 89.0f)
			{
				rotY -= _pitch - 89.0f;
				_pitch = 89.0f;
			}
			else if (_pitch < -89.0f)
			{
				rotY -= _pitch - -89.0f;
				_pitch = -89.0f;
			}
			break;
		}
		case SIX_DOF:
		{
			up = glm::normalize(_orientation * glm::vec3(0.0f, 1.0f, 0.0f));
			right = glm::normalize(_orientation * glm::vec3(1.0f, 0.0f, 0.0f));
			forward = glm::normalize(_orientation * glm::vec3(0.0f, 0.0f, -1.0f));
			qRoll = glm::angleAxis(glm::radians(roll), forward);
			break;
		}
		case FPS:
		{
			roll = 0;
			up = glm::vec3(0.0f, 1.0f, 0.0f);
			right = glm::vec3(1.0f, 0.0f, 0.0f);
			forward = glm::vec3(0.0f, 0.0f, -1.0f);
			qRoll = glm::angleAxis(glm::radians(0.0f), forward);
			if (_pitch > 89.0f)
			{
				rotY -= _pitch - 89.0f;
				_pitch = 89.0f;
			}
			else if (_pitch < -89.0f)
			{
				rotY -= _pitch - -89.0f;
				_pitch = -89.0f;
			}
			break;
		}
	}

	qPitch = glm::angleAxis(glm::radians(rotY), right);
	qYaw = glm::angleAxis(glm::radians(rotX), up);

	if (_type == SIX_DOF)
		_orientation = qYaw * qRoll * qPitch * _orientation;
	else
		_orientation = qYaw * qRoll * _orientation * qPitch;

	_orientation = glm::normalize(_orientation);
}

glm::vec3 Camera::computeForward() const
{
	glm::vec3 forward;
	forward = glm::normalize(_orientation * glm::vec3(0.0f, 0.0f, -1.0f));

	return forward;
}

glm::vec3 Camera::computeRight() const
{
	glm::vec3 right;
	right = glm::normalize(_orientation * glm::vec3(1.0f, 0.0f, 0.0f));

	return right;
}

glm::vec3 Camera::computeUp() const
{
	if (_type != SIX_DOF)
		return _altitude;
	glm::vec3 up;
	up = glm::normalize(_orientation * glm::vec3(0.0f, 1.0f, 0.0f));

	return up;
}

glm::mat4 Camera::computeView() const
{
	switch (static_cast<int>(_type))
	{
		case EULER :
		{
			glm::mat4 view = glm::lookAt(_position, _position + computeForward(), glm::vec3(0.0f, 1.0f, 0.0f));
			return(view);
		}
		case SIX_DOF :
		{
			glm::mat4 rot = glm::mat4_cast(glm::conjugate(_orientation));
			glm::mat4 trans = glm::translate(glm::mat4(1.0f), -_position);
			return(rot * trans);
		}
		case FPS :
		{
			glm::mat4 view = glm::lookAt(_position, _position + computeForward(), glm::vec3(0.0f, 1.0f, 0.0f));
			return(view);
		}
		default: return (glm::mat4());
	}
}

glm::vec3 Camera::getEulerAngles() const
{
    glm::vec3 euler = glm::eulerAngles(_orientation);
    euler = glm::degrees(euler);

    return euler;
}

void Camera::setCameraType(CameraType type)
{
	if (type == FPS)
		Logger::log(ENVIRONMENT, WARNING, "FPS camera pas fini");
	if (type != SIX_DOF)
	{
		_pitch = 0.0f;
		glm::quat pitch = glm::angleAxis(glm::radians(0.0f), glm::vec3(1, 0, 0));
		glm::quat yaw = glm::angleAxis(glm::radians(-90.0f), glm::vec3(0, 1, 0));
		glm::quat roll = glm::angleAxis(glm::radians(0.0f), glm::vec3(0, 0, -1));
		_orientation = yaw * roll * pitch;
	}
	_type = type;
}

void Camera::addPipelineToRender(PipelineType pipelineType)
{
	std::vector<PipelineType>::iterator pipelineit = std::find(_pipelines.begin(), _pipelines.end(), pipelineType);
	if (pipelineit == _pipelines.end())
		_pipelines.push_back(pipelineType);
}

void Camera::removePipelineToRender(PipelineType pipelineType)
{
	std::vector<PipelineType>::iterator pipelineit = std::find(_pipelines.begin(), _pipelines.end(), pipelineType);
	if (pipelineit != _pipelines.end())
		_pipelines.erase(pipelineit);
}

void Camera::renderViewMatrix(AEngine * engine, bool resized)
{
	static glm::vec3 oldPosition = _position;
	static glm::vec3 oldAltitude = _altitude;
	static glm::quat oldOrientation = _orientation;
	static float oldFov = -1;
	static EngineType oldEngineType = engine->getEngineType();
	EngineType engineType = engine->getEngineType();
	bool onVulkan = engineType == VULKAN;
	float nearPlane = 0.01f * (onVulkan ? 1 : NEAR_PLANE_OFFSET);

	if (resized || oldPosition != _position || oldAltitude != _altitude || oldOrientation != _orientation || oldFov != _fov || oldEngineType != engineType)
	{
		oldPosition = _position;
		oldAltitude = _altitude;
		oldOrientation = _orientation;
		oldFov = _fov;
		oldEngineType = engineType;
		_cameraBuffer.view = computeView();
		_cameraBuffer.proj = glm::perspective(glm::radians(_fov),
			static_cast<float>(_width) / static_cast<float>(_height),
			nearPlane, 1500.0f);
		if (onVulkan)
			_cameraBuffer.proj[1][1] *= -1;
	}
	for (PipelineType pipelineType : _pipelines)
		engine->updateUniformBuffer(PipelineManager::getPipeline(pipelineType).id, 0, &_cameraBuffer, sizeof(CameraBuffer));
}
