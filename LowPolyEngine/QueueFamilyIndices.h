#ifndef QUEUE_FAMILY_INDICES_H
#define QUEUE_FAMILY_INDICES_H

#include <array>

struct QueueFamilyIndices
{
	uint32_t graphicsFamily = -1;
	uint32_t presentFamily = -1;

	bool IsComplete() const
	{
		return graphicsFamily >= 0 && presentFamily >= 0;
	}

	std::array<uint32_t, 2> GetAsArray() const
	{
		return { graphicsFamily, presentFamily };
	}
};

#endif