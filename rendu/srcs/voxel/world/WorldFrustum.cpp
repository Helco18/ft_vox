#include "World.hpp"

void World::_extractPlanesFromProjmat(Camera * camera)
{
	glm::mat4 projmat = camera->getView();
	for (int f = 0; f <= FrustumDir::FRUSTUM_TOP; ++f)
	{
		for (int i = 0; i < 4; ++i)
		{
			if (f == FrustumDir::FRUSTUM_RIGHT)
				_planes[f].plane[i] = projmat[i][3] + projmat[i][0];
			else if (f == FrustumDir::FRUSTUM_LEFT)
				_planes[f].plane[i] = projmat[i][3] - projmat[i][0];
			else if (f == FrustumDir::FRUSTUM_TOP)
				_planes[f].plane[i] = projmat[i][3] + projmat[i][1];
			else if (f == FrustumDir::FRUSTUM_BOTTOM)
				_planes[f].plane[i] = projmat[i][3] - projmat[i][1];
		}
		glm::vec3 normal;
		normal = glm::vec3(_planes[f].plane[0], _planes[f].plane[1], _planes[f].plane[2]);
		if (normal.value)
		{
			float len = glm::length(normal);
			_planes[f].plane[0] /= len;
			_planes[f].plane[1] /= len;
			_planes[f].plane[2] /= len;
			_planes[f].plane[3] /= len;
		}
	}
}

static float getSignedDistanceToPlane(const glm::vec3 & pos, const Plane & p)
{
	glm::vec3 normal(p.plane[0], p.plane[1], p.plane[2]);
	return(glm::dot(normal, pos) + (p.plane[3]));
}

bool World::_chunkIsFrustum(Chunk * chunk)
{
	glm::vec3 min = chunk->getMin();
	glm::vec3 max = chunk->getMax();

	glm::vec3 c = (min + max) * 0.5f;
	glm::vec3 e = (max - min) - 0.5f;

	for (int f = 0; f <= FrustumDir::FRUSTUM_TOP; ++f)
	{
		const glm::vec3 normal(_planes[f].plane[0], _planes[f].plane[1], _planes[f].plane[2]);
		float r = e[0] * glm::abs(normal[0]) + e[1] * glm::abs(normal[1]) + e[2] * glm::abs(normal[2]);
		float s = getSignedDistanceToPlane(c, _planes[f]);
		if (s + r < 0)
			return false;
	}
	return true;
}
