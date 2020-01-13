#ifndef Object_hpp
#define Object_hpp

#include <vector>

#include "Model3D.hpp"
#include "Transform.hpp"

#include "glm/gtc/type_ptr.hpp"

namespace gps {

	class Object: public Model3D
	{

	public:
		Object();
		Object(std::string fileName, std::string basePath);

		void Draw(gps::Shader shaderProgram, glm::mat4 view);

		void addTransform(Transform transform);
		void clearTransforms();

	private:
		std::vector<Transform> transforms;
	};
}

#endif /* Model3D_hpp */
