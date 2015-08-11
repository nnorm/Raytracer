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
		float tmin = -999999999999.9f;
		float tmax =  999999999999.9f;

		vec3 bmin = b->center - b->extent;
		vec3 bmax = b->center + b->extent;

		if (r.rd.x != 0.0f)
		{
			float tx1 = (bmin.x - r.ro.x) / r.rd.x;
			float tx2 = (bmax.x - r.ro.x) / r.rd.x;

			tmin = max(tmin, min(tx1, tx2));
			tmax = min(tmax, max(tx1, tx2));
		}

		if (r.rd.y != 0.0f)
		{
			float ty1 = (bmin.y - r.ro.y) / r.rd.y;
			float ty2 = (bmax.y - r.ro.y) / r.rd.y;

			tmin = max(tmin, min(ty1, ty2));
			tmax = min(tmax, max(ty1, ty2));
		}

		if (r.rd.z != 0.0f)
		{
			float tz1 = (bmin.z - r.ro.z) / r.rd.z;
			float tz2 = (bmax.z - r.ro.z) / r.rd.z;

			tmin = max(tmin, min(tz1, tz2));
			tmax = min(tmax, max(tz1, tz2));
		}

		Intersection i;
		i.intersects = (tmax >= tmin);

		if (i.intersects)
		{
			i.position = r.ro + tmin * r.rd;

			vec3  dpos  = i.position - b->center;
			float dposx = abs(dot(dpos, vec3(1.0f, 0.0f, 0.0f)));
			float dposy = abs(dot(dpos, vec3(0.0f, 1.0f, 0.0f)));
			float dposz = abs(dot(dpos, vec3(0.0f, 0.0f, 1.0f)));

			float dposmax = max(dposx, max(dposy, dposz));
			if (dposmax == dposx)
				i.normal = vec3(sign(dot(dpos, vec3(1.0f, 0.0f, 0.0f))), 0.0f, 0.0f);
			if (dposmax == dposy)
				i.normal = vec3(0.0f, sign(dot(dpos, vec3(0.0f, 1.0f, 0.0f))), 0.0f);
			if (dposmax == dposz)
				i.normal = vec3(0.0f, 0.0f, sign(dot(dpos, vec3(0.0f, 0.0f, 1.0f))));
		}
		else
		{
			if (tmax >= 0.0f)
			{
				i.intersects = true;
				i.position = r.ro + tmax * r.rd;

				vec3  dpos = i.position - b->center;
				float dposx = abs(dot(dpos, vec3(1.0f, 0.0f, 0.0f)));
				float dposy = abs(dot(dpos, vec3(0.0f, 1.0f, 0.0f)));
				float dposz = abs(dot(dpos, vec3(0.0f, 0.0f, 1.0f)));

				float dposmax = max(dposx, max(dposy, dposz));
				if (dposmax == dposx)
					i.normal = vec3(sign(dot(dpos, vec3(1.0f, 0.0f, 0.0f))), 0.0f, 0.0f);
				if (dposmax == dposy)
					i.normal = vec3(0.0f, sign(dot(dpos, vec3(0.0f, 1.0f, 0.0f))), 0.0f);
				if (dposmax == dposz)
					i.normal = vec3(0.0f, 0.0f, sign(dot(dpos, vec3(0.0f, 0.0f, 1.0f))));
			}
		}

		return i;
	}
	
};
