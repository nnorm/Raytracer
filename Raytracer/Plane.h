#pragma once

#include "Common.h"
#include "Object.h"

struct Plane : 
	public Object
{
	Plane() : Object(ObjectType::PLANE) {}
	glm::vec3 n;
	float d;
};