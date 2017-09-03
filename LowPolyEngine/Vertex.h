#ifndef VERTEX_H
#define VERTEX_H

#include <vulkan/vulkan.hpp>

#include <vector>
#include <array>

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

namespace lpe
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 color;
		glm::vec2 texCoord;

		static vk::VertexInputBindingDescription getBindingDescription()
		{
			return { 0, sizeof(Vertex), vk::VertexInputRate::eVertex };
		}

		static decltype(auto) getAttributeDescriptions()
		{
			std::array<vk::VertexInputAttributeDescription, 3> descriptions =  { };
			descriptions[0] = { 0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, position) };
			descriptions[1] = { 1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color) };
			descriptions[2] = { 2, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, texCoord) };

			return descriptions;
		}

		bool operator==(const Vertex& other) const {
			return position == other.position && color == other.color && texCoord == other.texCoord;
		}
	};
}

namespace std
{
	template<> 
	struct hash<lpe::Vertex> {
		size_t operator()(lpe::Vertex const& vertex) const {
			return ((hash<glm::vec3>()(vertex.position) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}

#endif