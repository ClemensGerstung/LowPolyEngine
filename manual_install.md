### Libraries

You need at least:

1. Vulkan
2. GLFW
3. glm
4. stb_lib

It's recommended to create a *Libraries* folder where you put all of the needed libraries.  
So you can find them easily. I use *D:\Libraries* where **ALL** external libraries are.  
But in the end it's up to you.

#### Vulkan

Vulkan is a new graphics API deveoped by the Khronos Grp.

**Download**: https://vulkan.lunarg.com/

Select your system, download the newest version and install it whereever you want (but remember the path).  

Open your file explorer, navigate to %vulkan%/Bin and execute *Cube.exe* (or whatever it is called on your system if it's not Windows)

If it opens a window and displays a spinning cube you're good to go. Otherwise check Vulkan compatibility of your graphicscard and install if needed the drivers.

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
2. In **Linker**|**General** check **Additional Libraries Directories**
  1. You must link **Vulkan** and **GLFW**
    1. For Vulkan you have to link the **Source\lib** (or **Source\lib32** if x86) and the **Lib** (or **Lib32** if x86) folder (because of some validation layer stuff)
    2. For GLVFW select your VS (or other compiler) prebuild folder and select this (if you're using VS17 like me, just link the VS15 folder, works fine)
  2. Open the "Edit" Window of the "Libraries directories" and add new lines and select the library dir of the lib.
     In my case it lools like this:
      * D:\Libraries\Vulkan\Lib
      * D:\Libraries\Vulkan\Source\lib
      * D:\Libraries\glfw\lib-vc2015
3. In **Linker**|**Input** check **Additional Dependencies**
  1. You must link the static libraries of Vulkan and GLFW
  2. Open the "Edit" Window and enter:
      * vulkan-1.lib
      * glfw3.lib

**Congrats your done with the setup!**

You may also check in **General** the options **Target Platform Version** and **Platform Toolset** if they're set correctly for your VisualStudio version.