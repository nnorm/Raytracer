#pragma  once
#include "Common.h"
#include "Intersection.h"
#include "Camera.h"

using namespace glm;
struct Light
{
	glm::vec3 position;
	glm::vec3 color;
	float size;
	float intensity;

	Light() : position(vec3(0.0f)), color(vec3(1.0f)), size(0.0f), intensity(10.0f) {}

	float ComputeDiffuse(Intersection& i) const
	{
		vec3 L = normalize(position - i.position);
		vec3 N = i.normal;
		float diffuse = max(0.0f, dot(N, L));
		return diffuse;
	}

	float ComputeSpecular(const Intersection& i, const vec3& ro, float shininess)
	{
		vec3 L = normalize(position - i.position);
		vec3 N = i.normal;
		
		vec3 V = normalize(ro - i.position);
		vec3 H = normalize(L + V);

		float specular = max(0.0f, dot(N, H));
		specular = pow(specular, shininess);
		return specular;
	}
};