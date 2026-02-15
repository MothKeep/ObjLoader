# objLoader.h

is single file header that parses given .obj file. 
### User functions
Main function is **loadObject(vector\<Object\> Objects, vector\<Material\> Materials, string path)** which takes in arguments:
- std::vector\<objLoader::Object\> Objects - where all objects from given file are loaded.
- std::vector\<objLoader::Material> Materials - where all materials are loaded.
- std::string path - path to .obj file which is to be loaded.

If file was loaded succesfully true is returned or false otherwise.
For information on used structures look into objLoader.h (top of file).

# Renderer.h
is single file header that loads objects and materials to gpu, and renders them.
### User functions
Function loading object to gpu:
**LoadObject(objLoader::Object Object, std::vector\<objLoader::Material\> Materials)**
Arguments:
- objLoader::Object Object - model which is to be rendered.
- std::vector\<objLoader::Material\> Materials - vector of materials used in rendered object.

Function returns Renderer::Model - struct containing all pointers to loaded gpu data (via vector of meshes).
 
**void RenderObject(Model model, Shader shader, GLint\* SetMesh, std::vector\<objLoader::Material\> Materials)**
Arguments:
- Renderer::Model model - render-ready struct containing pointers to all gpu-loaded data
- Shader shader - shader program which is to be used when rendering object (look Shader.h)
- GLint\* SetMesh - Array of pointers to uniform locations which set object parameters. 
- std::vector\<objLoader::Material\> Materials - vector of materials used to render given object.

For information on used structures look into objLoader.h and Renderer.h (top section of both files).
