#include "Player.hpp"

TargetedBlock Player::getTargetedBlock() const
{
	if (!_world || !_world->isLoaded())
		return {BlockFace::BOTTOM, glm::vec3(0.0f), BlockType::AIR};
	glm::vec3 dir = _camera->computeForward(0);
	return _world->rayCast(_camera->getPosition(), dir, 5.0f);
}
