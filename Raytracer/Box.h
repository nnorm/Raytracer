#pragma once

#include "Common.h"
#include "Object.h"

struct Box :
	public Object
{
	Box() : Object(ObjectType::BOX), center(vec3(0.0f)), extent(vec3(1.0f)) {}
	vec3 center;
	vec3 extent;
};