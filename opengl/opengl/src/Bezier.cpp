#include "Bezier.hpp"
#include "glm/gtx/integer.hpp"

namespace gps {
	Bezier::Bezier()
	{

	}

	Bezier::Bezier(std::vector<glm::vec3> controlPoints) {
		this->controlPoints = controlPoints;
	}

	glm::vec3 Bezier::getPoint(float t)
	{
		int len = this->controlPoints.size();
		glm::vec3 point(0.0f);

		for (int i = 0; i < len; i++) {
			int j = len - i - 1;
			int comb = glm::factorial(len - 1) / (glm::factorial(j) * glm::factorial(i));

			point += comb * pow((1 - t), j) * pow(t, i) * this->controlPoints[i];
 		}

		return point;
	}
}
