#pragma once

#include <GL/glew.h>

#include "objLoader.h"
#include "shader.h"
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "stb_image.h"

namespace Renderer {

struct Mesh{
  GLuint VAO;
  GLuint VBO;
  GLsizei indexCount;
  std::string material;
  unsigned int textureID;
  unsigned int state;  // 0 - just vertices;  1 - vertices and texture 2 - vertices and normals 3 - all
};

struct Model{
  std::vector<Mesh> meshes;
};

unsigned int loadTexture(std::string path){
    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    int width, height, nrComponents;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    if (data){
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

Model LoadObject(objLoader::Object Object, std::vector<objLoader::Material>& Materials){
  Model model;
  for(const objLoader::Mesh& mesh : Object.meshes) {
    Mesh gpuMesh;
    gpuMesh.state = 0;
    if(Object.texCoords.size() > 0) gpuMesh.state+=1; 
    if(Object.normals.size() > 0) gpuMesh.state+=2; 

    glGenVertexArrays(1, &gpuMesh.VAO);
    glGenBuffers(1, &gpuMesh.VBO);

    glBindVertexArray(gpuMesh.VAO);

    std::vector<float> interleaved;
    interleaved.reserve(mesh.positions.size() * 8);

    for (size_t i = 0; i < mesh.positions.size(); i++){
      unsigned int v = mesh.positions[i];
      unsigned int n = (i < mesh.normPositions.size()) ? mesh.normPositions[i] : 0;
      unsigned int t = (i < mesh.texPositions.size()) ? mesh.texPositions[i] : 0;
      interleaved.push_back(Object.vertices[v * 3 + 0]);
      interleaved.push_back(Object.vertices[v * 3 + 1]);
      interleaved.push_back(Object.vertices[v * 3 + 2]);

      if (!Object.normals.empty()) {
        interleaved.push_back(Object.normals[n * 3 + 0]);
        interleaved.push_back(Object.normals[n * 3 + 1]);
        interleaved.push_back(Object.normals[n * 3 + 2]);
      } else {
        interleaved.push_back(0.0f);
        interleaved.push_back(1.0f);
        interleaved.push_back(0.0f);
      }
      if (!Object.texCoords.empty()) {
        interleaved.push_back(Object.texCoords[t * 3 + 0]);
        interleaved.push_back(Object.texCoords[t * 3 + 1]);
      } else {
        interleaved.push_back(0.0f);
        interleaved.push_back(1.0f);
      }
    }
        
    glBindBuffer(GL_ARRAY_BUFFER, gpuMesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, interleaved.size() * sizeof(float), interleaved.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    gpuMesh.indexCount = static_cast<GLsizei>(mesh.positions.size());
    gpuMesh.material = mesh.mtl;

    if(gpuMesh.state == 1 || gpuMesh.state == 3){
      objLoader::Material mtl = Materials[0]; 

      for(const objLoader::Material& mat : Materials){
        if(gpuMesh.material == mat.name){
          mtl=mat;
          break;
        } 
      }
      if(mtl.DiffuseMap != ""){
        int pos = 0;
        while ((pos = mtl.DiffuseMap.find("\\\\", pos)) != std::string::npos)
          mtl.DiffuseMap.replace(pos, 2, "/");
        gpuMesh.textureID = loadTexture(mtl.DiffuseMap);
      }
    }

    model.meshes.push_back(gpuMesh);

    glBindVertexArray(0);
  }
  return model;
}

void RenderObject(Model& model, Shader& shader, GLint* SetMesh, std::vector<objLoader::Material>& Materials){
  glUseProgram(shader.ID);
    
  for(const Mesh& mesh : model.meshes) {
    objLoader::Material mtl = Materials[0]; //Default

    for(const objLoader::Material& mat : Materials){
      if(mat.name == mesh.material){
        mtl=mat;
        break;
      } 
    }
  // 0 - just vertices;  1 - vertices and textures 2 - vertices and normals 3 - all

    glUniform3f(SetMesh[0], mtl.ambient[0], mtl.ambient[1], mtl.ambient[2] );
    glUniform3f(SetMesh[1], mtl.diffuse[0], mtl.diffuse[1], mtl.diffuse[2] );
    glUniform3f(SetMesh[2], mtl.specular[0], mtl.specular[1], mtl.specular[2] );
    glUniform1f(SetMesh[3], mtl.sExponent); 
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mesh.textureID);
    glUniform1i(SetMesh[4], 0);

    glUniform1i(SetMesh[5], mesh.state);
    
    glBindVertexArray(mesh.VAO);
    glDrawArrays(GL_TRIANGLES, 0, mesh.indexCount);
    glBindVertexArray(0);
  }
}

void DestroyModel(Model& model) {
  for (auto& mesh : model.meshes) {
    glDeleteVertexArrays(1, &mesh.VAO);
    glDeleteBuffers(1, &mesh.VBO);
  }
  model.meshes.clear();
}


}//close namespace
