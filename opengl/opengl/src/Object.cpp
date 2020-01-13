#include "Object.hpp"

#include "glm/glm.hpp"//core glm functionality
#include "glm/gtc/matrix_transform.hpp"//glm extension for generating common transformation matrices
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace gps {
	Object::Object() {}
	Object::Object(std::string fileName, std::string basePath) : Model3D(fileName, basePath) {}

	void Object::Draw(gps::Shader shaderProgram, glm::mat4 view) {
		glm::mat4 modelTransform(1.0f);

		for (auto it : transforms) modelTransform *= it.getMatrix();

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.shaderProgram, "model"),
			1,
			GL_FALSE,
			glm::value_ptr(modelTransform));

		//compute normal matrix
		glm::mat3 normalMatrix = glm::mat3(glm::inverseTranspose(view * modelTransform));
		//send normal matrix data to shader
		glUniformMatrix3fv(glGetUniformLocation(shaderProgram.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));

		Model3D::Draw(shaderProgram);
	}

	void Object::addTransform(Transform transform) {
		transforms.push_back(transform);
	}

	void Object::clearTransforms() {
		transforms.clear();
	}
}