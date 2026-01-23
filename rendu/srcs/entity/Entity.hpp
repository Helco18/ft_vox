#pragma once

#include "Camera.hpp"
#include "World.hpp"

class Entity
{
	public:
		Entity(const glm::vec3 & position = glm::vec3(0.0f)): _position(position) {};
		virtual ~Entity() {};

		const glm::vec3 &	getPosition() const { return _position; }
		Camera *			getCamera() { return _camera; }
		World *				getWorld() { return _world; }

		void				setCamera(Camera * camera) { _camera = camera; }
		void				setWorld(World * world) { _world = world; }

	private:
		glm::vec3	_position;
		Camera *	_camera;
		World *		_world;

};
