# LowPolyEngine

Simple render engine written in C++ and Vulkan as graphics API.  
Will be optimized to render low poly games like this:  
https://twitter.com/HumpaLumpa007/status/683763245493137408

## Wanted features

Fell free to complete this list

1. Simple API to create a window and add items to render
2. API to handle user inputs

## Wanted technical features

Feel free to complete this list either

1. Use SLI/Crossfire (why not?)
2. Render in HDR10 (because fuck myself but it may be worth it)
3. Global illumination (...)
4. High contrast (my be included in HDR10, but not all monitors have this feature yet)
5. Multi-Core CPU usage (because it's awesome)

## Setup project

You may need this section because VisualStudio can fuck you up during setup:

### Libraries

You need at least:

1. Vulkan
2. GLFW
3. glm

Additional libraries (not needed yet, and may be replaced):

4. stb_lib
5. tinyobjloader

It's recommended to create a *Libraries* folder where you put all of the needed libraries.  
So you can find them easily. I use *D:\Libraries* where **ALL** external libraries are.  
But in the end it's up to you.

#### Vulkan

Vulkan is a new graphics API deveoped by the Khronos Grp.

**Download**: https://vulkan.lunarg.com/

Select your system, download the newest version and install it whereever you want (but remember the path).  

Open your file explorer, navigate to %vulkan%/Bin and execute *Cube.exe* (or whatever it is called on your system if it's not Windows)

If it opens a window and displays a spinning cube you're good to go. Otherwise check Vulkan compatibility of your graphicscard and install if needed the neede drivers.

#### GLFW

GLFW is a C API to create and display windows cross plattform. It also has Vulkan support by default, which is pretty nice.

**Download**: http://www.glfw.org/download.html

If you're on Windows, you may want to download the prebuild libs for you target platform (x86, x64).

Extract the zip file into your libraries folder and your good to go.

#### glm

glm is a math library, because you don't want to write the whole stuff on your own. And it's also well tested and works fine.

**Download**: https://github.com/g-truc/glm/tags

Download the latest version from their git repo and extract it into your libraries folder.

glm is a "header-only" library, so there are only .h files and no .lib or .dll to link to. Keep that in mind for later.

#### stb_lib

stb_lib is a large collection of "single header" libaries to do various things.

**Download**: https://github.com/nothings/stb

Download the zip from the git repo and extract it into your libraries folder.

Like glm stb_lib is a "header-only" library

#### tinyobjloader

tinyobjloader is a libary to load .obj files (3D models).

**Download**: https://github.com/syoyo/tinyobjloader

Download the zip from the git repo and extract it into your libraries folder.

Like glm and stb_lib tinyobjloader is a "header-only" library

### Installation

**I use VisualStudio 2017 Community Edition to develop this library.**

Clone this repository into your projects folder and open the .sln file.

Now setup the paths to include and link directories. But first set **LowPolyEngine.Test** as *startup project* and run the project.

This may fail but is needed to enable a option in the properties window of the project (thx VS).

For **LowPolyEngine** just do Step 1 and for **LowPolyEngine.Test** do all steps.  
Also don't forget to link LowPolyEngine to LowPolyEngine.Test so you can use the lib itself.

1. In **C/C++**|**General** check **Additional Include Directories**
  1. You must at least link **Vulkan**, **GLFW** and **glm**
  2. You may also link **stb_lib** and **tinyobjloader**. Though they're not needed yet
  3. Open the "Edit" Window of the "Include directories" and add new lines and select the include dir of the lib.  
     In my case it looks like this:
      * D:\Libraries\Vulkan\Include
      * D:\Libraries\glm
      * D:\Libraries\glfw\include
      * D:\Libraries\stb_lib
      * D:\Libraries\tinyobjloader
2. In **Linker**|**General** check **Additional Libraries Directories**
  1. You must link **Vulkan** and **GLFW**
    1. For Vulkan you have to link the **Bin** (or **Bin32** if x86) and the **Lib** (or **Lib32** if x86) folder (because of some validation layer stuff)
    2. For GLVFW select your VS (or other compiler) prebuild folder and select this (if you're using VS17 like me, just link the VS15 folder, works fine)
  2. Open the "Edit" Window of the "Libraries directories" and add new lines and select the library dir of the lib.
     In my case it lools like this:
      * D:\Libraries\Vulkan\Lib
      * D:\Libraries\Vulkan\Bin
      * D:\Libraries\glfw\lib-vc2015
3. In **Linker**|**Input** check **Additional Dependencies**
  1. You must link the static libraries of Vulkan and GLFW
  2. Open the "Edit" Window and enter:
      * vulkan-1.lib
      * glfw3.lib

**Congrats your done with the setup!**

You may also check in **General** the options **Target Platform Version** and **Platform Toolset** if they're set correctly for your VisualStudio version.


## Clion Setup

### Under Windows

1. Install Vulkan SDK see *Vulkan*
2. Install MSys2 http://www.msys2.org/
3. Install a toolchain: *pacman -S mingw-w64-x86_64-toolchain*
4. Select the mingw installation in CLion:
    * File | Settings | Build ,Execution... | Toolchains
    * Set MinGW Home to {MSys2 installation dir}\mingw64