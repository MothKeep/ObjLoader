# ObjLoader
is a combination of 2 main parts - .obj file parser, and example Renderer written in OpenGL. 
Application takes in any .obj file, parses data inside and renders it real-time. 
# Usage
To start application (load and render object) install and compile project, 
then from project directory run:
./objLoader \<filename\> *[0|1]
where filename is path to .obj file you want to use, and [0|1] is optional argument which sets flip textures to true or false.
## Dependencies:
- CMake
- OpenGL
- GLFW3 
- stb_image (already in src)
- glm (already in src)
## Installation:
move to directory you want to install ObjLoader in then:

git clone github.com/MothKeep/ObjLoader
cd ObjLoader
cmake .
make

With that you should have executable program ObjLoader. 
