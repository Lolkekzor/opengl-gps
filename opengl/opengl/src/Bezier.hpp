#ifndef Bezier_hpp
#define Bezier_hpp

#include <stdio.h>
#include <vector>
#include "glm/glm.hpp"

namespace gps {
	class Bezier
	{
	public:
		Bezier();
		Bezier(std::vector<glm::vec3> controlPoints);

		glm::vec3 getPoint(float t);

	private:
		std::vector<glm::vec3> controlPoints;
	};

}

#endif
