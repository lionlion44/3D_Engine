#include "commonUtil.h"
#include "geoStructs.h"

#pragma once

// Color Functions
vec3d rgbToHsv(vec3d& rgb);
vec3d hsvToRgb(vec3d& hsv);

// Vector Math Functions
vec3d cross(vec3d& vec1, vec3d& vec2);
vec3d normalize(vec3d vec);
float dot(vec3d vec1, vec3d vec2);
void rotate(vec3d& vec, float x, float y, float z);
void scale(vec3d& vec, float x, float y, float z);
void translate(vec3d& vec, float x, float y, float z);
void transform(vec3d& vec, float rx, float ry, float rz, float sx, float sy, float sz, float tx, float ty, float tz);

// Util Functions
template <typename T>
auto enumerate(std::vector<T>& vec);

// Matrix Functions
void matMult4d(vec4d& vec, matrix4d& m);
void matMult3d(vec3d& vec, matrix3d& m);
void project(vec3d& vec, matrix4d& m);