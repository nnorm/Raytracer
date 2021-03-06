#pragma once
#include "Common.h"
#include "Material.h"

enum class ObjectType 
{
	PLANE,
	SPHERE,
	BOX,
	UNDEFINED
};
struct Object
{
	Object() : type(ObjectType::UNDEFINED) {}
	Object(ObjectType type) : type(type) {}
	ObjectType type;
	Material material;
};
