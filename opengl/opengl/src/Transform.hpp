#ifndef Transform_hpp
#define Transform_hpp

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

namespace gps {

	enum TRANSFORM_TYPE { TRANSLATE, ROTATE, SCALE };

	class Transform
	{
	public:
		Transform(TRANSFORM_TYPE type, glm::vec3 vec = glm::vec3(0.0f), float angle = 0.0f);
		glm::mat4 getMatrix();

	private:
		TRANSFORM_TYPE type;
		glm::vec3 vec;
		float angle;
	};

}

#endif
