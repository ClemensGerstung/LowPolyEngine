In this file I'll describe my thoughts about this engine, what it's supposed to be and what it may be become.

## Wanted features

### Simple API to create a window and add items to render

As a client of this lib I want a simple interface to create a Window (which will be a ```GLFWwindow``` internally).  
To show the window just call the ```Render()``` function in a loop while the Window is open.

```
// constructor params: title, width, height, resizeable
lpe::Window window = { "Hello Vulkan lpe", 800, 600, false };

while(window.IsOpen())
{
    // add things to render path...
    
    window.Render();
}
```

This is also the current status quo.

To add items to render there should be a method like ```AddRender{Object}(...)``` or ```Draw{Object}(...)``` *(I'm not sure about that)*.  
These Methods should have some overloads *(not overlords)*.

#### 2D Methods

**The X and Y position are the absolut pixel position of the window and (0|0) is in the upper left corner.**

* ```DrawRect(width / 2 - 50, height / 2 - 50, 100, 100)```: Renders a rectangle on the screen at the x|y-position and width and height.
* ```DrawCircle(width / 2 - 50, height / 2 - 50, 100)```:  Renders a circle on the screen at the x|y-position and diameter
* ```DrawText(100, 100, "Hello World")```: Renders the given text at the given x|y-position
* ```DrawVertices({ {50, 50}, {100, 50}, {100, 100}, {50, 100} }, true)```: Renders the collection of vertices on the screen, last parameter is a ```bool``` indicating if the form should be closed.

These methods also have overloads which take a ```glm::vec2``` as position parameter.  
Like: ```DrawRect({ { width / 2 - 50, height / 2 - 50 }, 100, 100})```.  
Also all methods have a optional parameter ```type``` which will be set to ```lpe::DrawType::Line```.
Other option is ```lpe::DrawType::Fill```.

#### 3D Methods

* ```AddRenderObject({"../assets/objects/tree.obj", "../assets/textures/tree.jpg"})```: This will render a 3D object on the screen. It'll create a new C++ object of type ```RenderObject```. It'll also be rendered in the center of the screen because (0|0|0) is in the center. Also keep in mind that Vulkan coorination system is from *-1* to *1*. So your model should be normaliyed to this size.
* ```AddRenderVertices({ {0.5, 0.5, 0.5}, {0.75, 0.5, 0.5}, {0.75, 0.75, 0.5}, {0.5, 0.75, 0.5} }, true)```: This works like the 2D version but every vertex needs a Y-position. Keep also in mind that the position goes frm *-1* to *1*. This method need to triangulate the vertices to get the render indices to push it properly to the graphics card

2D and 3D methods can be combined.

#### Handle user input

I'm not sure about that section at all.

One way is to create for every input it's own handler. Like this:
```
window.HandleInput(lpe::Key::W, [](KeyInputEvent e)
{
    // move camera foreward
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
            // move camera foreward
        }
    }
}
```

This should also handle inputs of a gamepad.  
This calls will be forewarded to GLFW which has such a simple input handler.

### Technical features

In this section I want to describe and discuss some technical features of the 

#### SLI/Crossfire (multiple GPU support)

This might be a bit tricky because first I have to pick every GPU with vulkan support and not only the first one which is suitable.  

The "easiest" way would be to create for each physical device a logical device and a swapchain. Build for each device the render chain and 
in ```DrawFrame``` cramp all swapchains into the ```vk::PresentInfoKHR``` (or ```VkPresentInfoKHR```).

If this (or another way which I maybe find in the future) would work it would not really matter which GPUs (different vendors, different generations) you put into your computer i'll just work.
Of course as long as they support vulkan.

#### HDR10

Well it seems that in the latest version of Vulkan (or the LunarG SDK) it's pretty easy to accomplish that.  
You have to check the ```vk::Format``` and ```vk::ColorSpaceKHR``` enums for 10bit Format and for the HDR10 ColorSpace.  
But there are two HDR10 ColorSpaces which I have to determine what they are supposed to do (I think I need an HDR10 device to check :( ).

#### Global illumination

I have no idea how to do this. I want this feature because the test renders in Cinema4D look awesome with GI enabled.  
More on that in the future...

#### High contrast

This should be a "simple" shader which will be applied during rendering. This is only needed if the monitor doesn't have HDR10.

#### Multicore CPU usage

Because quadcore CPUs seem to be the most used CPU types for now it makes sense to use (almost) all cores.  
I don't know yet how to accomplish this but maybe the *boost* lib can help here.

## Libraries

As I wrote in the readme.md ```Vulkan```, ```GLFW``` and ```glm```. Vulkan because it's the graphics API, GLFW because I don't want to write for each system (Window, Linux, Mac) a own window and glm because I don't want to write the whole math stuff.  
These libs (mostly GLFW and glm) are well tested and are used pretty often. Writing this stuff by your own would be nice but also more likly to fail.

It's not sure if I use stb_lib and tinyobjloader in the future but I think so.  

Also boost my be required in the future.
