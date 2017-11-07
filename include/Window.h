#ifndef WINDOW_H
#define WINDOW_H

#include "stdafx.h"
#include "Instance.h"
#include "Commands.h"
#include <glm/detail/type_vec3.hpp>
#include "Camera.h"

BEGIN_LPE
	class Window
	{
	private:
		GLFWwindow* window = nullptr;
		// using raw pointer because glfw is an c api and std::unique_ptr wouldn't work due to incomplete type...
		uint32_t width;
		uint32_t height;
		std::string title;
		bool resizeable;
		lpe::Camera defaultCamera;
		lpe::Instance instance;
		lpe::Device device;
		lpe::SwapChain swapChain;
		lpe::Commands commands;
		lpe::UniformBuffer uniformBuffer;
		lpe::Pipeline graphicsPipeline;
		lpe::ImageView depthImage;
		lpe::ModelsRenderer modelsRenderer;
    lpe::RenderPass renderPass;

	protected:
		virtual void Create();
    static void KeyInputCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	public:
		Window() = default;
		Window(const Window& window) = delete;
		Window(Window&& window) = delete;
		Window operator=(const Window& window) const = delete;
		Window operator=(Window&& window) const = delete;

		Window(uint32_t width, uint32_t height, std::string title, bool resizeable = false);
		virtual ~Window();

		// TODO: add functions for imgui stuff and further methods to preinit window!
		void Create(uint32_t width, uint32_t height, std::string title, bool resizeable = false);

		lpe::Camera CreateCamera(glm::vec3 position, glm::vec3 lookAt = {0, 0, 0}, float fov = 60, float near = 0.0,
		                         float far = 10) const;

		lpe::Model* AddModel(std::string path);

		bool IsOpen() const;

		void Render();
	};

END_LPE

#endif
