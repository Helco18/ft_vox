#include "Camera.hpp"

void Camera::_extractPlanes()
{
	const glm::mat4 & projmat = _view;
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
