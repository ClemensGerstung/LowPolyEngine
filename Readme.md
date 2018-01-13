# LowPolyEngine

Simple render engine written in C++ and Vulkan as graphics API.  
Will be optimized to render low poly games like this:  
https://twitter.com/HumpaLumpa007/status/683763245493137408

Always feel free to complete the feature lists!

See [my thoughts](somethoughts.md) about this engine (might be outdated).

## Wanted Main Features

### 1. Simple API to create a window and add items to render

You can create a ```lpe::Window``` and add some ```lpe::RenderObjects``` to it.  
These Objects can have several instances which are rendered separately (see [instancing](https://en.wikipedia.org/wiki/Geometry_instancing)).

#### Example:
```
#include "lpe.h"

int main()
{
    lpe::settings.EnableValidationLayer = false;    // enable if you want to see Vulkan messages which may accur
    
    lpe::RenderObject object = { "models/cube.ply", 0 };    // create a lpe::RenderObject with a renderprio set to 0. 
                                                            // the renderprio will be used later
                                                            
    uint32_t instances = 5;
    
    // create 25 instances of this object which are rendered in a 5x5 grid
    for (uint32_t x = 0; x < instances; ++x)
    {
      for (uint32_t y = 0; y < instances; ++y)
      {
        auto instance = object.GetInstance(x * instances + y);
        instance->SetPosition({ x, y, 0 });
        instance->SetTransform(glm::scale(glm::mat4(1), { 0.75f, 0.75f, 0.75f }));
      }
    }

    lpe::Window window;
    
    try
    {
        window.Create(1920, 1080, "LowPolyEngine Sample", false);   // you could also use the second ctor to create a window
                                                                    // params: width, height, title, resizeable
        window.AddRenderObject(&object);    // add RenderObject so each instance will be rendered
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        while (window.IsOpen())
        {
          auto currentTime = std::chrono::high_resolution_clock::now();
          float time = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count() / 2500.0f;
        
          for (uint32_t x = 0; x < instances; ++x)
          {
            for (uint32_t y = 0; y < instances; ++y)
            {
              // rotate all instances
              // you can access each instance on it's own and rotate with different speed for example
              auto instance = object.GetInstance(x * instances + y);
              instance->SetPosition({ x, y, 0 });
              instance->SetTransform(glm::scale(glm::mat4(1), { 0.75f, 0.75f, 0.75f }));
              instance->Transform(glm::rotate(glm::mat4(1), glm::radians(90.0f) * time, { 0, 0, 1 }));
            }
          }
        
          window.Render();  // render...
        }
    }
    catch (std::runtime_error e)
    {
        std::cerr << e.what() << std::endl; // if there is any error
    
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
```

### 2. API to handle user inputs

##### I'm not sure about this at all.

One way is to create for every input it's own handler. Like this:
```
window.HandleInput(lpe::Key::W, [](KeyInputEvent e)
{
    // move camera forward
});
```

Other way is to create a own class which inherits from something like ```InputHandlerBase```. This would look like:
```
class SimpleInputHandler : public InputHandlerBase 
{
public:
    void HandleInput(KeyInputEvent e) override
    {
        if(e.Key == lpe::Key::W && e.Type == lpe::InputType::KeyDown)
        {
            // move camera forward
        }
    }
}
```

This should also handle inputs of a gamepad.  
This calls will be forwarded to GLFW which has such a simple input handler, but needs to be wrapped.

### Your Ideas

... tbd

## Wanted technical features

### 1. Use SLI/Crossfire

Nope! [See](https://www.lunarg.com/faqs/scalable-link-interface-sli-vulkan/)

### 2. Render in HDR10 

Might be a nice feature for those 1% who have a monitor which supports HDR10. But it seems that they get more common this year (2018)

### 3. Global illumination 

I have no idea how to do this. I want this feature because the test renders in Cinema4D look awesome with GI enabled.  
More on that in the future...

### 4. High contrast 

This should be a "simple" shader which will be applied during rendering. This is only needed if the monitor doesn't have HDR10.

### 5. Multi-Core CPU usage

Because quadcore CPUs seem to be the most used CPU types for now it makes sense to use (almost) all cores.  
Even Intel's newest mainstream CPUs (80XX Series) have six cores. 
I don't know yet how to accomplish this but maybe the *boost* lib can help here.

### 6. Your Ideas

... tbd

## Setup project

You may need this section because VisualStudio can fuck you up during setup:

If you want to do all of this manually see here: [manual_install.md]

#### Install Vulkan

Vulkan is a new graphics API deveoped by the Khronos Grp.

**Download**: https://vulkan.lunarg.com/

Select your system, download the newest version and install it whereever you want (but remember the path).  

Open your file explorer, navigate to %vulkan%/Bin and execute *Cube.exe* (or whatever it is called on your system if it's not Windows)

If it opens a window and displays a spinning cube you're good to go. Otherwise check Vulkan compatibility of your graphicscard and install if needed the drivers.

#### Download Project

Clone this project to your local disk:  
```git clone https://github.com/HumpaLumpa007/LowPolyEngine.git```

#### Load Subdependencies

```git submodule init```  
```git submodule update```

#### Setup

##### Visual Studio

Create the solution file with:  
```cmake -G "Visual Studio 15 Win64"```

This will link all libraries (vulkan, glfw, glm).
Make sure you use the correct Visual Studio version in the cmake command (15 = Visual Studio 2017)

##### CLion (under Windows)

1. Install Vulkan SDK see [Install Vulkan](#vulkan)
2. Install MSys2 ([http://www.msys2.org/])
3. Install a toolchain: ```pacman -S mingw-w64-x86_64-toolchain```
4. Select the mingw installation in CLion:
    * File | Settings | Build ,Execution... | Toolchains
    * Set MinGW Home to {MSys2 installation dir}\mingw64

#### Open Project

Open the project with your IDE and create a build.

If you're using Visual Studio make sure that the LowPolyEngine.Test Project is set to startup project!


## What's next?

My current schedule is:
1. Implement multi pipeline rendering (use the ```lpe::RenderObject::prio```, the objects will be rendered on different pipelines by their priority) ✅
2. Add [ImGUI](https://github.com/ocornut/imgui) support - [see](https://github.com/SaschaWillems/Vulkan/tree/master/examples/imgui)
2. Implement frustum culling and lod - [see](https://github.com/SaschaWillems/Vulkan/tree/master/examples/computecullandlod)
3. Tessellation - [see](https://github.com/SaschaWillems/Vulkan/tree/master/examples/terraintessellation)
4. Shadows - [see](https://github.com/SaschaWillems/Vulkan/tree/master/examples/shadowmappingcascade)
5. Begin with wanted technical features