#ifndef VDELETER_H
#define VDELETER_H

#include <vulkan/vulkan.h>
#include <functional>

/*
in case somebody wonder how to lambda in c++11

here are the start definitions of a lambda (from http://www.cprogramming.com/c++11/c++11-lambda-closures.html)
[]	Capture nothing (or, a scorched earth strategy?)
[&]	Capture any referenced variable by reference
[=]	Capture any referenced variable by making a copy
[=, &foo]	Capture any referenced variable by making a copy, but capture variable foo by reference
[bar]	Capture bar by making a copy; don't copy anything else
[this]	Capture the this pointer of the enclosing class

*/
namespace lpe {
	template <typename T>
	class VDeleter
	{
	public:
		VDeleter() : VDeleter([](T, VkAllocationCallbacks*) {})
		{
		}

		VDeleter(std::function<void(T, VkAllocationCallbacks*)> deletef)
		{
			this->deleter = [=](T obj)
			{
				deletef(obj, nullptr);
			};
		}

		VDeleter(const VDeleter<VkInstance>& instance, std::function<void(VkInstance, T, VkAllocationCallbacks*)> deletef)
		{
			this->deleter = [&instance, deletef](T obj)
			{
				deletef(instance, obj, nullptr);
			};
		}

		VDeleter(const VDeleter<VkDevice>& device, std::function<void(VkDevice, T, VkAllocationCallbacks*)> deletef)
		{
			this->deleter = [&device, deletef](T obj)
			{
				deletef(device, obj, nullptr);
			};
		}

		~VDeleter()
		{
			cleanup();
		}

		T* replace()
		{
			cleanup();
			return &object;
		}

		const T* operator &() const
		{
			return &object;
		}

		operator T() const
		{
			return object;
		}

		void operator=(T rhs)
		{
			if (rhs != object)
			{
				cleanup();
				object = rhs;
			}
		}

	private:
		T object{ VK_NULL_HANDLE };
		std::function<void(T)> deleter;

		void cleanup()
		{
			if (object != VK_NULL_HANDLE)
			{
				deleter(object);
			}

			object = VK_NULL_HANDLE;
		}
	};
}

#endif
