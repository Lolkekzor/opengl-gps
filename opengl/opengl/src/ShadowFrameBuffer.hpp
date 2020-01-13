#ifndef ShadowFrameBuffer_hpp
#define ShadowFrameBuffer_hpp

#include "GL/glew.h"

namespace gps {
	class ShadowFrameBuffer
	{
	public:
		ShadowFrameBuffer();

		GLuint shadowBuffer;
		GLuint shadowTexture;
	};
}

#endif