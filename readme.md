# Rotating Cube in OpenGL 4

<p align="center"><img src="https://raw.githubusercontent.com/c2d7fa/opengl-cube/master/screenshot.png" width="400"/></p>

This is a simple OpenGL 4.5 demo using GLEW and GLFW. It is written in pure C, and does not depend on any libraries for matrix manipulation.

I made this to learn about OpenGL. I used the following resources, all of which I recommend:

- [Anton's OpenGL 4 Tutorials](https://antongerdelan.net/opengl/) – Detailed tutorial on getting started with 2D.
- [Open.GL](https://open.gl/) – Concise introduction to 3D.
- [docs.GL](https://docs.gl/) – Nicer UI for the official reference documentation.
- [OpenGL Projection Matrix](http://www.songho.ca/opengl/gl_projectionmatrix.html) – Solid resource for understanding projection. Use this if you don't want to depend on GLM.

## Building

This demo depends on GLEW and GLFW. You also need OpenGL 4.5 or later. (It would probably work in any version of OpenGL 4 if you edit the version directive in the shaders, but I haven't checked.)

### Linux

Install GLEW and GLFW packages using your package manager, so that you have the liraries `glfw`, `GLEW` and `GL` available. Then execute the script `run.sh`, which will compile and run the application.

### Windows

These instructions are for Visual Studio 2019. Start by importing the project into Visual Studio.

Start by downloading the prebuilt packages for [GLEW](http://glew.sourceforge.net/) and [GLFW](https://www.glfw.org/download.html) (or build them yoursef).

Each of these packages should contain an `include` directory which contains subdirectories with some header files. Add each of the include directories under _VC++ Directories_ in the project properties.

Each package should also contain some `.lib` files. Add the directories with these library files in the same place.
