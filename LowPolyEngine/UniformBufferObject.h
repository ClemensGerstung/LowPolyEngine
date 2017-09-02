#ifndef UNIFOR_BUFFER_OBJECT_H
#define UNIFOR_BUFFER_OBJECT_H

#include <glm/glm.hpp>

namespace lpe
{
	struct UniformBufferObject
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};
}

#endif