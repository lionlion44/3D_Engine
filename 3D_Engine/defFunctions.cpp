#include "commonUtil.h"
#include "geoStructs.h"

vec3d rgbToHsv(vec3d& rgb) {
	float r = rgb.x / 255;
	float g = rgb.y / 255;
	float b = rgb.z / 255;

	float cmax = std::max(std::max(r, g), b);
	float cmin = std::min(std::min(r, g), b);
	float delta = cmax - cmin;

	float h = 0;
	if (delta != 0) {
		if (cmax == r) h = 60 * ((g - b) / delta);
		else if (cmax == g) h = 60 * ((b - r) / delta + 2);
		else if (cmax == b) h = 60 * ((r - g) / delta + 4);
	}
	if (h < 0) h += 360;

	float saturation = (cmax == 0) ? 0 : (delta / cmax);
	float value = cmax;

	return { h, saturation, value };
}

vec3d hsvToRgb(vec3d& hsv) {
	float hue = hsv.x;
	float saturation = hsv.y;
	float value = hsv.z;

	int hueSegment = static_cast<int>(hue / 60) % 6;
	float hueFractionalPart = hue / 60 - hueSegment;
	float chroma = value * saturation;
	float minComponent = value - chroma;

	float intermediateValue = chroma * (float)(1 - std::abs(std::fmod(hueSegment, 2) - 1));

	float r = 0, g = 0, b = 0;
	switch (hueSegment) {
	case 0: r = chroma; g = intermediateValue; b = 0; break;
	case 1: r = intermediateValue; g = chroma; b = 0; break;
	case 2: r = 0; g = chroma; b = intermediateValue; break;
	case 3: r = 0; g = intermediateValue; b = chroma; break;
	case 4: r = intermediateValue; g = 0; b = chroma; break;
	case 5: r = chroma; g = 0; b = intermediateValue; break;
	}

	vec3d rgb = { (r + minComponent) * 255, (g + minComponent) * 255, (b + minComponent) * 255 };

	return rgb;
}

vec3d cross(vec3d& vec1, vec3d& vec2)
{
	return { vec1.y * vec2.z - vec1.z * vec2.y,
			 vec1.z * vec2.x - vec1.x * vec2.z,
			 vec1.x * vec2.y - vec1.y * vec2.x };
}

vec3d normalize(vec3d vec)
{
	float length = sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
	return { vec.x / length, vec.y / length, vec.z / length };
}

float dot(vec3d vec1, vec3d vec2)
{
	return ((vec1.x * vec2.x) + (vec1.y * vec2.y) + (vec1.z * vec2.z));
}

template <typename T>
auto enumerate(std::vector<T>& vec) {
	std::vector<std::pair<size_t, T*>> result;
	for (size_t i = 0; i < vec.size(); ++i) {
		result.push_back({ i, &vec[i] });
	}
	return result;
}

void matMult4d(vec4d& vec, matrix4d& m)
{
	vec4d temp = vec;
	vec.x = temp.x * m.m[0][0] + temp.y * m.m[0][1] + temp.z * m.m[0][2] + temp.w * m.m[0][3];
	vec.y = temp.x * m.m[1][0] + temp.y * m.m[1][1] + temp.z * m.m[1][2] + temp.w * m.m[1][3];
	vec.z = temp.x * m.m[2][0] + temp.y * m.m[2][1] + temp.z * m.m[2][2] + temp.w * m.m[2][3];
	vec.w = temp.x * m.m[3][0] + temp.y * m.m[3][1] + temp.z * m.m[3][2] + temp.w * m.m[3][3];
}

void matMult3d(vec3d& vec, matrix3d& m)
{
	vec3d temp = vec;
	vec.x = temp.x * m.m[0][0] + temp.y * m.m[0][1] + temp.z * m.m[0][2];
	vec.y = temp.x * m.m[1][0] + temp.y * m.m[1][1] + temp.z * m.m[1][2];
	vec.z = temp.x * m.m[2][0] + temp.y * m.m[2][1] + temp.z * m.m[2][2];
}

void project(vec3d& vec, matrix4d& m)
{
	vec3d temp = vec;
	vec.x = temp.x * m.m[0][0] + temp.y * m.m[0][1] + temp.z * m.m[0][2] + m.m[0][3];
	vec.y = temp.x * m.m[1][0] + temp.y * m.m[1][1] + temp.z * m.m[1][2] + m.m[1][3];
	vec.z = temp.x * m.m[2][0] + temp.y * m.m[2][1] + temp.z * m.m[2][2] + m.m[2][3];
	float w = temp.x * m.m[3][0] + temp.y * m.m[3][1] + temp.z * m.m[3][2] + m.m[3][3];

	if (w != 0.0f)
	{
		vec.x /= w;
		vec.y /= w;
		vec.z /= w;
	}
}

void rotate(vec3d& vec, float x, float y, float z)
{
	// Declare Rotation Matrix
	matrix3d rotMatX, rotMatY, rotMatZ;
	{
		rotMatX.m[0][0] = 1;
		rotMatX.m[1][1] = cosf(x);
		rotMatX.m[1][2] = -sinf(x);
		rotMatX.m[2][1] = sinf(x);
		rotMatX.m[2][2] = cosf(x);

		rotMatY.m[0][0] = cosf(y);
		rotMatY.m[0][2] = sinf(y);
		rotMatY.m[1][1] = 1;
		rotMatY.m[2][0] = -sinf(y);
		rotMatY.m[2][2] = cosf(y);

		rotMatZ.m[0][0] = cosf(z);
		rotMatZ.m[0][1] = -sinf(z);
		rotMatZ.m[1][0] = sinf(z);
		rotMatZ.m[1][1] = cosf(z);
		rotMatZ.m[2][2] = 1;
	}

	matMult3d(vec, rotMatX);
	matMult3d(vec, rotMatY);
	matMult3d(vec, rotMatZ);
}

void scale(vec3d& vec, float x, float y, float z)
{
	vec = { vec.x * x, vec.y * y, vec.z * z };
}

void translate(vec3d& vec, float x, float y, float z)
{
	vec = { vec.x + x, vec.y + y, vec.z + z };
}

void transform(vec3d& vec, float rx, float ry, float rz, float sx, float sy, float sz, float tx, float ty, float tz)
{
	matrix4d rotX, rotY, rotZ, scale, trans, final;

	rotX = { { {1, 0, 0, 0},
				{0, cosf(rx), -sinf(rx), 0},
				{0, sinf(rx), cosf(rx), 0},
				{0, 0, 0, 1} } };
	rotY = { { {cosf(ry), 0, sinf(ry), 0},
				{0, 1, 0, 0},
				{-sinf(ry), 0, cosf(ry), 0},
				{0, 0, 0, 1} } };
	rotZ = { { {cosf(rz), -sinf(rz), 0, 0},
				{sinf(rz), cosf(rz), 0, 0},
				{0, 0, 1, 0},
				{0, 0, 0, 1} } };

	scale = { { {sx, 0, 0, 0},
				{0, sy, 0, 0},
				{0, 0, sz, 0},
				{0, 0, 0, 1} } };

	trans = { { {1, 0, 0, tx},
				{0, 1, 0, ty},
				{0, 0, 1, tz},
				{0, 0, 0, 1} } };

	final = rotX * rotY * rotZ * scale * trans;

	vec4d tempVec = { vec.x, vec.y, vec.z, 1 };

	matMult4d(tempVec, final);

	vec = { tempVec.x, tempVec.y, tempVec.z };
}