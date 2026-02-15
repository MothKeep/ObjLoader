# ObjLoader

**ObjLoader** consists of two main components:

- A **`.obj` file parser**
- An **example renderer written in OpenGL**

The application loads any `.obj` file, parses its data, and renders it in real time.


## Usage

To load and render an object:

1. Install and compile the project.
2. From the project directory, run:

```bash
./objLoader <filename> [0|1]*
```
### Arguments

- `<filename>`  
  Path to the `.obj` file you want to load.

- `[0|1]` *(optional)*  
  Controls texture flipping:
  - `0` → do not flip textures  
  - `1` → flip textures  
  - Defaults to `1` if omitted.


## Dependencies

- CMake  
- OpenGL  
- GLFW3  
- stb_image *(included in `src`)*  
- glm *(included in `src`)*  


## Installation

Move to the directory where you want to install ObjLoader, then run:

```bash
git clone https://github.com/MothKeep/ObjLoader
cd ObjLoader
cmake .
make
```
After building, the executable objLoader will be created in the project directory.
