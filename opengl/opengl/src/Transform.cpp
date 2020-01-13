#include "Transform.hpp"

namespace gps {
	Transform::Transform(TRANSFORM_TYPE type, glm::vec3 vec, float angle)
		:type(type), vec(vec), angle(angle) {}

	glm::mat4 Transform::getMatrix() {
		glm::mat4 matrix(1.0f);

		switch (type) {
			case TRANSLATE:
				matrix = glm::translate(matrix, vec);
				break;

			case ROTATE:
				matrix = glm::rotate(matrix, angle, vec);
				break;

			case SCALE:
				matrix = glm::scale(matrix, vec);
				break;
		}

		return matrix;
	}
}