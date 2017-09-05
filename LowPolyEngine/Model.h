#ifndef MODEL_H
#define MODEL_H

#include "Vertex.h"
#include <vector>

namespace lpe
{
	class Model
	{
		std::vector<lpe::Vertex> vertices;
		std::vector<uint32_t> indices;

	public:
		Model() = default;
		~Model() = default;

		void Load(std::string fileName);


		std::vector<lpe::Vertex> GetVertices() const;

		std::vector<uint32_t> GetIndices() const;
	};
}

#endif