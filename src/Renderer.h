#pragma once

#include <GL/glew.h>

#include "objLoader.h"
#include "shader.h"
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace Renderer {

struct Mesh{
  GLuint VAO;
  GLuint VBO;
  GLsizei indexCount;
  std::string material;
};

struct Model{
  std::vector<Mesh> meshes;
};

Model LoadObject(objLoader::Object Object){
  Model model;
  for(const objLoader::Mesh& mesh : Object.meshes) {
    Mesh gpuMesh;
    std::vector<float> interleaved;
    interleaved.reserve(mesh.positions.size() * 6);

    for (size_t i = 0; i < mesh.positions.size(); i++) {
      unsigned int v = mesh.positions[i];
      unsigned int n = (i < mesh.normPositions.size()) ? mesh.normPositions[i] : 0;
      
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
    }
    
    glGenVertexArrays(1, &gpuMesh.VAO);
    glGenBuffers(1, &gpuMesh.VBO);

    glBindVertexArray(gpuMesh.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, gpuMesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, interleaved.size() * sizeof(float), interleaved.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);


    gpuMesh.indexCount = static_cast<GLsizei>(mesh.positions.size());
    gpuMesh.material = mesh.mtl;
    model.meshes.push_back(gpuMesh);

    glBindVertexArray(0);
  }
  return model;
}

void RenderObject(Model& model, Shader& shader, GLint* SetCol, std::vector<objLoader::Material> Materials){
  glUseProgram(shader.ID);
    
  for(const Mesh& mesh : model.meshes) {
    objLoader::Material mtl = Materials[0]; //Default

    for(const objLoader::Material& mat : Materials){
      if(mat.name == mesh.material){
        mtl=mat;
        break;
      } 
    }

    glUniform3f(SetCol[0], mtl.ambient[0], mtl.ambient[1], mtl.ambient[2] );
    glUniform3f(SetCol[1], mtl.diffuse[0], mtl.diffuse[1], mtl.diffuse[2] );
    glUniform3f(SetCol[2], mtl.specular[0], mtl.specular[1], mtl.specular[2] );
    glUniform1f(SetCol[3], mtl.sExponent); 
    
    
    glBindVertexArray(mesh.VAO);
    glDrawArrays(GL_TRIANGLES, 0, mesh.indexCount);
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
