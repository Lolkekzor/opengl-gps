#include "ShadowFrameBuffer.hpp"

namespace gps {
	const GLuint SHADOW_WIDTH = 8192, SHADOW_HEIGHT = 8192;

	ShadowFrameBuffer::ShadowFrameBuffer() {
		//generate FBO ID
		glGenFramebuffers(1, &this->shadowBuffer);

		//create depth texture for FBO
		glGenTextures(1, &this->shadowTexture);
		glBindTexture(GL_TEXTURE_2D, this->shadowTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		//attach texture to FBO
		glBindFramebuffer(GL_FRAMEBUFFER, this->shadowBuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->shadowTexture, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}