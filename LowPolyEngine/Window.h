#ifndef WINDOW_H
#define WINDOW_H

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "SwapChain.h"
#include "GraphicsPipeline.h"
#include "Commands.h"

namespace lpe
{
    class Window
    {
        GLFWwindow *window; // using raw pointer because glfw is an c api and std::unique_ptr wouldn't work due to incomplete type...
        uint32_t width;
        uint32_t height;
        SwapChain swapChain;
		GraphicsPipeline pipeline;
		Commands commands;
		ImageView depthImageView;

    protected:
        virtual void
        InitWindow(uint32_t width, uint32_t height, const std::string &title, bool resizeable);

    public:
        Window() = delete;
        virtual ~Window();

        Window(uint32_t width,
               uint32_t height,
               std::string &&title,
               bool resizeable = true);

        Window(uint32_t width,
               uint32_t height,
               const std::string &title,
               bool resizeable = true);
        Window(const Window &window) = delete;

        Window(Window &&window) = delete;

        Window operator=(const Window &window) const = delete;
        Window operator=(Window &&window) const = delete;

        bool IsOpen() const;

        void Render();

        uint32_t GetHeight() const;

        uint32_t GetWidth() const;
    };
}

#endif
