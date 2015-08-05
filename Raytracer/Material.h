#pragma once

#include "Common.h"



struct Material
{
	vec3 albedo;
	vec3 F0;
	float reflectivity;
	float shininess;

	float refractionFactor;
	float IOR;

	Material() : albedo(vec3(1.0f)), F0(vec3(0.1f)), reflectivity(0.0f), shininess(64.0f), refractionFactor(0.0f), IOR(1.0f) {}
};