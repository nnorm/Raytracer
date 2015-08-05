#pragma once
#include "Common.h"
#include "glm/gtx/intersect.hpp"

#include "Ray.h"
#include "Object.h"
#include "Sphere.h"
#include "Plane.h"

struct Intersection
{
	float t;
	glm::vec3 position;
	glm::vec3 normal;
	bool intersects;
	Object* obj;

	Intersection() : t(99999999.0f), position(glm::vec3(0.0f)), normal(glm::vec3(0.0f)), intersects(false) {}

	bool operator<(const Intersection& b)
	{
		if (!intersects)
			return false;

		return t < b.t;
	}

	static Intersection RaySphere(const Ray& r, Sphere* s)
	{
		Intersection i;

		if (glm::intersectRaySphere(r.ro, r.rd, s->center, s->radius * s->radius, i.t))
		{
			i.intersects = true;
			i.position = r.ro + i.t * r.rd;
			i.normal = glm::normalize(i.position - s->center);
			i.obj = s;
		}

		return i;
	}

	static Intersection RayPlane(const Ray& r, Plane* p)
	{
		Intersection i;
		i.t = -(glm::dot(r.ro, p->n) + p->d) / glm::dot(r.rd, p->n);
		if (i.t >= 0.0f)
		{
			i.intersects = true;
			i.position = r.ro + i.t * r.rd;
			i.normal = p->n;
			i.obj = p;
		}

		return i;
	}

	//TODO
	/*static Intersection RayBox(const Ray& r, Box* b)
	{}*/
};
