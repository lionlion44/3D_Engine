#include "commonUtil.h"

#pragma once

struct vec3d
{
	float x, y, z;

	vec3d operator - (const vec3d other) const
	{
		return { x - other.x,  y - other.y,  z - other.z };
	}

	vec3d operator + (const vec3d other) const
	{
		return { x + other.x,  y + other.y,  z + other.z };
	}

	vec3d operator * (const vec3d other) const
	{
		return { x * other.x,  y * other.y,  z * other.z };
	}
};

struct vec4d
{
	float x, y, z, w;
};

struct triangle
{
	vec3d pts[3];
	int id;
	vec3d color;
};

struct points
{
	std::vector<vec3d> pts;
};

struct matrix4d
{
	float m[4][4] = { 0 };

	matrix4d operator * (matrix4d other) const
	{
		matrix4d result = { { {0,0,0,0},
							  {0,0,0,0},
							  {0,0,0,0},
							  {0,0,0,0} } };

		for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) for (int k = 0; k < 4; k++) result.m[i][j] += m[i][k] * other.m[k][j];
		return result;
	}
};

struct matrix3d
{
	float m[3][3] = { 0 };
};

struct mesh
{
	std::vector<triangle> tris, trisTranformed;
	matrix4d tranform = { { { 1, 0, 0, 0 },
							{ 0, 1, 0, 0 },
							{ 0, 0, 1, 0 },
							{ 0, 0, 0, 1 } } };

	bool loadFromObjFile(std::string sFilename)
	{
		std::ifstream f(sFilename);
		if (!f.is_open())
			return false;

		// Local cache of verts
		std::vector<vec3d> verts;

		while (!f.eof())
		{
			char line[128];
			f.getline(line, 128);

			std::strstream s;
			s << line;

			char junk;

			if (line[0] == 'v')
			{
				vec3d v;
				s >> junk >> v.x >> v.y >> v.z;
				verts.push_back(v);
			}

			if (line[0] == 'f')
			{
				int f[3];
				s >> junk >> f[0] >> f[1] >> f[2];
				tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
			}
		}

		return true;
	}
};