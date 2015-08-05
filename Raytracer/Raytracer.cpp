#include "Raytracer.h"
#include "Intersection.h"
#include "Light.h"

#define NB_SHADOW_SAMPLE 256

Raytracer::Raytracer(int width, int height)
	: _width(width),
	  _height(height)
{
	_data = new Pixel[_width * _height];
	for (int y = 0; y < _height; y++)
	{
		for (int x = 0; x < _width; x++)
		{
			_data[y * _width + x].rgba[0] = 200;
			_data[y * _width + x].rgba[1] = 200;
			_data[y * _width + x].rgba[2] = 200;
			_data[y * _width + x].rgba[3] = 255;
		}
	}
}

Raytracer::~Raytracer()
{
	delete[] _data;
}

void* Raytracer::getData()
{
	return _data;
}

void Raytracer::trace(int x, int y, const Ray& r, int maxReflectLevel)
{
	vec3 lighting = traceSingleRay(r, 0, maxReflectLevel);

	/* Sending color data */
	_data[y * _width + x].rgba[0] = (unsigned char)(lighting.x * 255.0f);
	_data[y * _width + x].rgba[1] = (unsigned char)(lighting.y * 255.0f);
	_data[y * _width + x].rgba[2] = (unsigned char)(lighting.z * 255.0f);
	_data[y * _width + x].rgba[3] = 255;
}

glm::vec3 Raytracer::traceSingleRay(const Ray& r, int currentReflectLevel, int maxReflectLevel)
{
	Intersection i;
	for (Object* o : _objects)
	{
		Intersection iCurrent;

		switch (o->type)
		{
		case ObjectType::PLANE:
			iCurrent = Intersection::RayPlane(r, static_cast<Plane*>(o));
			break;

		case ObjectType::SPHERE:
			iCurrent = Intersection::RaySphere(r, static_cast<Sphere*>(o));
			break;

		default:
			std::cout << "Unknown shape" << std::endl;
			assert(false);
			break;
		}

		if (iCurrent < i)
			i = iCurrent;
	}

	vec3 result = vec3(0.784f);

	if (i.intersects)
	{
		result = glm::clamp(_computeLighting(i, r.ro), glm::vec3(0.0f), glm::vec3(1.0f));
		if (i.obj->material.reflectivity > 0.0f && currentReflectLevel < maxReflectLevel)
		{
			vec3 fresnel = glm::mix(i.obj->material.F0, vec3(1.0f), max(0.0f, powf(1.0f - glm::dot(-r.rd, i.normal), 5.0f)));

			Ray reflectedRay;
			reflectedRay.rd = reflect(r.rd, i.normal);
			reflectedRay.ro = i.position + 0.01f * reflectedRay.rd;
			vec3 reflectedColor = traceSingleRay(reflectedRay, currentReflectLevel+1, maxReflectLevel);

			
			//reflectedColor *= fresnel;
			//reflectedColor += (vec3(1.0f) - reflectedColor) * result;

			result = mix(result, reflectedColor, i.obj->material.reflectivity * fresnel);
		}
	}

	return result;
}

glm::vec3 Raytracer::_computeLighting(Intersection& i, const glm::vec3& ro)
{
	glm::vec3 result(0.0f);
	for (auto light : _lights)
	{
		float diffuse = light->ComputeDiffuse(i);
		float specular = light->ComputeSpecular(i, ro, i.obj->material.shininess);
		float d = glm::distance(i.position, light->position) - light->size;
		float attenuation = light->intensity / (1.0f + d*d);
		float shadowFactor = _traceShadows(i, light);
		result += (i.obj->material.albedo * diffuse * (1.0f - specular) + vec3(specular)) * light->color * attenuation * (1.0f - shadowFactor);
	}
	return result;
}

float Raytracer::_traceShadows(Intersection& i, Light* light)
{
	/* Shadowing */
	float shadowCoeff = 0.0f;

	#pragma omp parallel for
	for (int sample = 0; sample < NB_SHADOW_SAMPLE; sample++)
	{
		Ray rs;

		float rand1 = float((rand() % 2048) - 1024) / 1024.0f;
		float rand2 = float((rand() % 2048) - 1024) / 1024.0f;
		float rand3 = float((rand() % 2048) - 1024) / 1024.0f;

		glm::vec3 randomSphere = glm::normalize(vec3(rand1, rand2, rand3));
		vec3 randomPos = light->position + randomSphere * light->size;
		rs.rd = glm::normalize(randomPos - i.position);
		rs.ro = i.position + 0.02f * rs.rd;
	
		Intersection i;
		for (Object* o : _objects)
		{
			Intersection iCurrent;

			switch (o->type)
			{
			case ObjectType::PLANE:
				iCurrent = Intersection::RayPlane(rs, static_cast<Plane*>(o));
				break;

			case ObjectType::SPHERE:
				iCurrent = Intersection::RaySphere(rs, static_cast<Sphere*>(o));
				break;

			default:
				std::cout << "Unknown shape" << std::endl;
				assert(false);
				break;
			}

			if (iCurrent < i)
				i = iCurrent;
		}

		if (i.intersects)
		{
			if (i.t < glm::distance(i.position, light->position))
				shadowCoeff += 1.0f;
		}
	}

	shadowCoeff /= float(NB_SHADOW_SAMPLE);
	return shadowCoeff;
}

void Raytracer::addObject(Object* obj)
{
	_objects.push_back(obj);
}

void Raytracer::addLight(Light* light)
{
	_lights.push_back(light);
}