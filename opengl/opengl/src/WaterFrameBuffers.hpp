#ifndef WaterFrameBuffers_hpp
#define WaterFrameBuffers_hpp

#include "GL/glew.h"

namespace gps {
	class WaterFrameBuffers
	{
	public:
		WaterFrameBuffers();

		GLuint reflectionBuffer;
		GLuint refractionBuffer;

		GLuint reflectionTexture;
		GLuint refractionTexture;
	};
}

#endif