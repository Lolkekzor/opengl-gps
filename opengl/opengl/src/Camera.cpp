//
//  Camera.cpp
//  Lab5
//
//  Created by CGIS on 28/10/2016.
//  Copyright © 2016 CGIS. All rights reserved.
//

#include "Camera.hpp"

#include <iostream>
#include <algorithm>
#include "glm/gtx/vector_angle.hpp"

namespace gps {
    
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget)
    {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(this->cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
		this->cameraDirectionAfterPitch = this->cameraDirection;
		this->pitch = this->yaw = 0;
    }
    
    glm::mat4 Camera::getViewMatrix()
    {
		if (this->mode == 0)
			return glm::lookAt(cameraPosition, cameraPosition + cameraDirectionAfterPitch, glm::vec3(0.0f, 1.0f, 0.0f));
		else
			return glm::lookAt(cameraPosition, cinematicTarget, glm::vec3(0.0f, 1.0f, 0.0f));
    }

	glm::vec3 Camera::getCameraTarget()
	{
		return cameraTarget;
	}

	glm::vec3 Camera::getCameraPosition()
	{
		return this->cameraPosition;
	}
    
    void Camera::move(MOVE_DIRECTION direction, float speed)
    {
        switch (direction) {
            case MOVE_FORWARD:
                cameraPosition += cameraDirectionAfterPitch * speed;
                break;
                
            case MOVE_BACKWARD:
                cameraPosition -= cameraDirectionAfterPitch * speed;
                break;
                
            case MOVE_RIGHT:
                cameraPosition += cameraRightDirection * speed;
                break;
                
            case MOVE_LEFT:
                cameraPosition -= cameraRightDirection * speed;
                break;

			case MOVE_UP:
				cameraPosition += glm::vec3(0, 1, 0) * speed;
				break;

			case MOVE_DOWN:
				cameraPosition -= glm::vec3(0, 1, 0) * speed;
				break;
        }
		std::cout << cameraPosition.x << ' ' << cameraPosition.y << ' ' << cameraPosition.z << '\n';
    }

	void Camera::flip()
	{
		this->rotate(-1000 * this->pitch, 0);
	}
    
	void Camera::switchMode()
	{
		if (this->mode == 0) // if camera is being controlled
		{
			this->mode = 1;
			this->cinematicStep = 0;
		}
		else 
		{
			this->mode = 0;
			this->pitch = 0.126;
			//this->yaw = 3.154;
			this->yaw = 0;
			this->rotate(0, 0);
		}
	}

	void Camera::updateCinematic(float deltaTime)
	{
		if (this->mode == 0)
			return;

		const float cinematicSpeed = 0.05;
		this->cinematicStep += cinematicSpeed * deltaTime;
		this->cameraPosition = cinematicPath.getPoint(cinematicStep);

		if (this->cinematicStep > 1)
			this->switchMode();
	}

	void Camera::setCinematic(std::vector<glm::vec3> points, glm::vec3 target)
	{
		this->cinematicPath = Bezier(points);
		this->cinematicTarget = target;
	}

    void Camera::rotate(float pitch, float yaw)
    {
		const double speed = 0.002;
		pitch *= speed;
		yaw *= speed;

		this->pitch += pitch;
		this->pitch = std::min(this->pitch, 1.3);
		this->pitch = std::max(this->pitch, -1.3);
		this->yaw += yaw;

		glm::mat4 matrix(1.0f);
		matrix = glm::rotate(matrix, yaw, glm::vec3(0, -1, 0));
		glm::vec4 aux = matrix * glm::vec4(this->cameraDirection, 0.0f);
		this->cameraDirection = glm::vec3(aux.x, aux.y, aux.z);
		this->cameraRightDirection = glm::normalize(glm::cross(this->cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f)));

		matrix = glm::mat4(1.0f);
		matrix = glm::rotate(matrix, (float)this->pitch, -this->cameraRightDirection);
		aux = matrix * glm::vec4(this->cameraDirection, 0.0f);
		this->cameraDirectionAfterPitch = glm::vec3(aux.x, aux.y, aux.z);
    }
}
