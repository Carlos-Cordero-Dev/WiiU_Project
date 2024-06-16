#pragma once

#include <glm/glm.hpp>
#include <vpad/input.h>

glm::vec3 VpadToGlm(VPADVec3D& v)
{
	return { v.x, v.y , v.z };
}