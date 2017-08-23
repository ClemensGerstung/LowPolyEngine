#ifndef QUEUE_FAMILY_INDICES_H
#define QUEUE_FAMILY_INDICES_H

struct QueueFamilyIndices
{
	uint32_t graphicsFamily = -1;
	uint32_t presentFamily = -1;

	bool isComplete() const
	{
		return graphicsFamily >= 0 && presentFamily >= 0;
	}
};

#endif