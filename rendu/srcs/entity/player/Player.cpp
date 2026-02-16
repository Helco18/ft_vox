#include "Player.hpp"

PosFace Player::getTargetedBlock() const
{
	glm::vec3 dir = _camera->computeForward(0);
	return _world->rayCast(_camera->getPosition(), dir, 5.0f);
}
