//
//  Camera.hpp
//
//  Created by CGIS on 28/10/2016.
//  Copyright © 2016 CGIS. All rights reserved.
//

#ifndef Camera_hpp
#define Camera_hpp

#include <stdio.h>
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "Bezier.hpp"

namespace gps {
    
    enum MOVE_DIRECTION {MOVE_FORWARD, MOVE_BACKWARD, MOVE_RIGHT, MOVE_LEFT, MOVE_UP, MOVE_DOWN};
    
    class Camera
    {
    public:
        Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget);
        glm::mat4 getViewMatrix();
		glm::vec3 getCameraTarget();
		glm::vec3 getCameraPosition();
		void move(MOVE_DIRECTION direction, float speed);
		void rotate(float pitch, float yaw);
		void flip();
		void switchMode();
		void setCinematic(std::vector<glm::vec3> points, glm::vec3 target);
		void updateCinematic(float deltaTime);

    private:
        glm::vec3 cameraPosition;
        glm::vec3 cameraTarget;
        glm::vec3 cameraDirection;
        glm::vec3 cameraRightDirection;
		glm::vec3 cameraDirectionAfterPitch;

		glm::vec3 cinematicTarget;
		Bezier cinematicPath;

		double pitch, yaw;
		float cinematicStep;
		int mode = 0; // 0 - controlled, 1 - cinematic
    };
    
}

#endif /* Camera_hpp */
