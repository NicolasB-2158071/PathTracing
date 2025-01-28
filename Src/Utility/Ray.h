#pragma once

#include <embree4/rtcore.h>
#include <glm/glm.hpp>

inline RTCRayHit CreateRayHit(
	const glm::vec3& origin,
	const glm::vec3& dir,
	float tnear = 0.0f,
	float tfar = std::numeric_limits<float>::infinity(),
	int mask = -1,
	unsigned int geomID = RTC_INVALID_GEOMETRY_ID,
	unsigned int primID = RTC_INVALID_GEOMETRY_ID
) {
	RTCRayHit rayHit;
	rayHit.ray.org_x = origin.x; rayHit.ray.org_y = origin.y; rayHit.ray.org_z = origin.z;
	rayHit.ray.dir_x = dir.x; rayHit.ray.dir_y = dir.y; rayHit.ray.dir_z = dir.z;
	rayHit.ray.tnear = tnear;
	rayHit.ray.tfar = tfar;
	rayHit.ray.mask = mask;
	rayHit.hit.geomID = geomID;
	rayHit.hit.primID = primID;
	return rayHit;
}