#pragma once

#include "Entity.hpp"

class Player: public Entity
{
	public:
		Player(const glm::vec3 & position = glm::vec3()): Entity(position) {}

		TargetedBlock	getTargetedBlock() const;

};
