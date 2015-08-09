#pragma once
#include "Common.h"
#include "glm/gtx/intersect.hpp"

#include "Ray.h"
#include "Object.h"
#include "Sphere.h"
#include "Plane.h"
#include "Box.h"

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
			i.normal = normalize(p->n);
			i.obj = p;
		}

		return i;
	}

	//TODO
	
	static Intersection RayBox(const Ray& r, Box* b)
	{
		Intersection i;
		float tx1 = (b->minimum.x - r.ro.x) * (1.0f / r.rd.x);
		float tx2 = (b->maximum.x - r.ro.x) * (1.0f / r.rd.x);

		float tmin = min(tx1, tx2);
		float tmax = max(tx1, tx2);

		float ty1 = (b->minimum.y - r.ro.y) * (1.0f / r.rd.y);
		float ty2 = (b->maximum.y - r.ro.y) * (1.0f / r.rd.y);

		tmin = max(tmin, min(ty1, ty2));
		tmax = min(tmax, max(ty1, ty2));

		float tz1 = (b->minimum.z - r.ro.z) * (1.0f / r.rd.z);
		float tz2 = (b->maximum.z - r.ro.z) * (1.0f / r.rd.z);

		tmin = max(tmin, min(tz1, tz2));
		tmax = min(tmax, max(tz1, tz2));

		i.intersects = tmax >= tmin;

		i.position = r.ro + ((tmax >= tmin) ? tmax: tmin) * r.rd;

		return i;
	}
	
};
