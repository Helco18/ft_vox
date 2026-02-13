#include "World.hpp"

// X
reyStat lookingUpX(double tan, const glm::vec3 & pos)
{
	reyStat stat;
	stat.rey.x = tan + pos.x;
	stat.rey.y = pos.y;
	stat.rey.z = pos.z;

	stat.o.x = tan;
	stat.o.y = -1;
	stat.o.z = -1;

	return stat;
}

reyStat lookingDownX(double tan, const glm::vec3 & pos)
{
	reyStat stat;
	stat.rey.x = pos.y - 1 * tan + pos.x;
	stat.rey.y = pos.y + 1;
	stat.rey.z = pos.z;

	stat.o.x = -tan;
	stat.o.y = 1;
	stat.o.z = 1;

	return stat;
}

//Y
reyStat lookingUpY(double tan, const glm::vec3 & pos)
{
	reyStat stat;
	stat.rey.x = pos.x;
	stat.rey.y = tan + pos.y;
	stat.rey.z = pos.z;
	
	stat.o.x = -1;
	stat.o.y = tan;
	stat.o.z = -1;

	return stat;
}

reyStat lookingDownY(double tan, const glm::vec3 & pos)
{
	reyStat stat;
	stat.rey.x = pos.x + 1;
	stat.rey.y = pos.x - 1 * tan + pos.y;
	stat.rey.z = pos.z;

	stat.o.x = 1;
	stat.o.y = -tan;
	stat.o.z = 1;

	return stat;
}

//Z
reyStat lookingUpZ(double tan, const glm::vec3 & pos)
{
	reyStat stat;
	stat.rey.x = pos.x;
	stat.rey.y = pos.y;
	stat.rey.z = tan + pos.z;
	
	stat.o.x = -1;
	stat.o.y = -1;
	stat.o.z = tan;

	return stat;
}

reyStat lookingDownZ(double tan, const glm::vec3 & pos)
{
	reyStat stat;
	stat.rey.x = pos.x + 1;
	stat.rey.y = pos.y;
	stat.rey.z = pos.x - 1 * tan + pos.z;

	stat.o.x = 1;
	stat.o.y = 1;
	stat.o.z = -tan;

	return stat;
}

double World::_ray(int dof, const glm::vec3 & pos, reyStat & stat)
{
	glm::vec3 mPos = stat.rey;
	while (dof < 1000)
	{
		if (getChunkAt(mPos)->getBlock(mPos.x, mPos.y, mPos.z) != 0)
		{
			return glm::distance(pos, mPos);
		}
		else
		{
			mPos += stat.o;
			dof++;
		}
	}
	return 10000000;
}

double World::_xLines(double ra,  const glm::vec3 & pos)
{
	int			dof = 0;
	double		tan;
	reyStat		stat;

	tan = -1 / glm::tan(ra);
	if (ra > glm::pi<double>())
		stat = lookingUpX(tan, pos);
	if (ra < glm::pi<double>())
		stat = lookingDownX(tan, pos);
	if (ra == 0 || ra == glm::pi<double>())
	{
		stat.o = pos;
		stat.rey = pos;
		dof = 1000;
	}
	return (_ray(dof, pos, stat));
}

double World::_yLines(double ra,  const glm::vec3 & pos)
{
	int			dof = 0;
	double		tan = 0;
	reyStat		stat;

	if (ra > glm::pi<double>())
		stat = lookingUpY(tan, pos);
	if (ra < glm::pi<double>())
		stat = lookingDownY(tan, pos);
	if (ra == 0 || ra == glm::pi<double>())
	{
		stat.o = pos;
		stat.rey = pos;
		dof = 1000;
	}
	return (_ray(dof, pos, stat));
}

double World::_zLines(double ra,  const glm::vec3 & pos)
{
	int			dof = 0;
	double		tan;
	reyStat		stat;

	tan = -glm::tan(ra);
	if (ra > glm::pi<double>())
		stat = lookingUpZ(tan, pos);
	if (ra < glm::pi<double>())
		stat = lookingDownZ(tan, pos);
	if (ra == 0 || ra == glm::pi<double>())
	{
		stat.o = pos;
		stat.rey = pos;
		dof = 1000;
	}
	return (_ray(dof, pos, stat));
}

PosFace World::_rayCast(const glm::vec3 & pos, const glm::vec3 & dir)
{
	double	ra = 0.0;
	double	distanceX = _xLines(ra, pos);
	double	distanceY = _yLines(ra, pos);
	double	distanceZ = _zLines(ra, pos);

	if (distanceX < distanceY && distanceX && distanceZ)
		Logger::log(VOXEL, INFO, "min distanc is X at" + toString(distanceX));
	if (distanceY < distanceX && distanceY && distanceZ)
		Logger::log(VOXEL, INFO, "min distanc is Y at" + toString(distanceX));
	if (distanceZ < distanceY && distanceZ && distanceY)
		Logger::log(VOXEL, INFO, "min distanc is Z at" + toString(distanceZ));
}
