@echo off

if [%VULKAN_SDK%] == [] goto ERROR_NO_VULKAN

for %%f in (shaders/*.vert, shaders/*.frag) do (
	%VULKAN_SDK%/Bin/glslangValidator.exe -V shaders/%%f -o shaders/%%f.spv
)

goto END

:ERROR_NO_VULKAN
echo VULKAN_SDK path not found
echo please define variable!

:END