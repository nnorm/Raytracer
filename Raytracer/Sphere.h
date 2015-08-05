#pragma once
#include "Common.h"
#include "Object.h"

struct Sphere
	: public Object
{
	Sphere() : Object(ObjectType::SPHERE) {}
	glm::vec3 center;
	float radius;
};