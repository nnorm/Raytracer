#pragma once
#include "Common.h"
#include "Object.h"
#include "Ray.h"
#include "Light.h"

class Raytracer
{
public:
	Raytracer(int width, int height);
	~Raytracer();

	void* getData();
	void trace(int x, int y, const Ray& r, int maxReflectLevel);
	vec3 traceSingleRay(const Ray& r, int currentReflectLevel, int maxReflectLevel);
	void addObject(Object* obj);
	void addLight(Light* light);

private:
	void _computeLighting(Intersection& i, const glm::vec3& ro, vec3& diffuseResult, vec3& specularResult);
	float _traceShadows(Intersection& i, Light* light);
	int _width;
	int _height;

#pragma pack(push, 1)
	struct Pixel
	{
		unsigned char rgba[4];
	};
	Pixel* _data;
#pragma pack(pop)

	std::vector<Object*> _objects;
	std::vector<Light*> _lights;
};
