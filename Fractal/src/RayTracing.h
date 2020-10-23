#ifndef EXTRA_H
#define EXTRA_H

#include <iostream>
#include <cstdint>
#include <cstdlib> 
#include <cmath>
#include <cassert>
#include <algorithm>

#include "LiteMath.h"

#define M_PI 3.141592653589793

const unsigned width = 1280, height = 720;

using namespace LiteMath;

float3 get_ray_color(const float3& ray_pos, const float3& ray_dir);

#endif 
