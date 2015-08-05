#pragma once
#include "Common.h"

struct Camera
{
private:
	glm::vec3 CamOrigin;
public:
	glm::mat4 viewMatrix;
	glm::mat4 projMatrix;
	bool originHasBeenComputed = false;
	
	glm::vec3 getO()
	{
		glm::vec4 origin;
		if (!originHasBeenComputed)
		{
			origin = glm::inverse(viewMatrix) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			CamOrigin = glm::vec3(origin.x, origin.y, origin.z);
		}
		else
			origin = glm::vec4(CamOrigin,1.0);
		
		return glm::vec3(origin);
	}

	glm::vec3 calculateRd(glm::vec2 ndcPos)
	{
		glm::vec4 tempPixel = glm::vec4(ndcPos.x, ndcPos.y, 0.0f, 1.0f);
		tempPixel = glm::inverse(projMatrix) * tempPixel;
		tempPixel /= tempPixel.w;
		tempPixel = glm::inverse(viewMatrix) * tempPixel;
		glm::vec3 re = glm::vec3(tempPixel);
		glm::vec3 rd = glm::normalize(re - getO());
		return rd;
	}

};