## Building

### Linux

Install GLEW and GLFW packages using your package manager, so that you have the liraries `glfw`, `GLEW` and `GL` available. Then execute the script `run.sh`, which will compile and run the application.

### Windows

These instructions are for Visual Studio 2019. Start by importing the project into Visual Studio.

Start by downloading the prebuilt packages for GLEW and GLFW.

Each of these packages should contain an `include` directory which contains subdirectories with some header files. Add each of the include directories under *VC++ Directories* in the project properties.

Each package should also contain some `.lib` files. Add the directories with these library files in the same place.

Finally, under *Linker* and then *Input* add the *Additional Dependencies* `glew32s.lib`, `glfw3.lib` and `opengl32.lib`.