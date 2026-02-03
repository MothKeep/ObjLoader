#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "objLoader.h"
#include "shader.h"

#define SCR_WIDTH 1920
#define SCR_HEIGHT 1080

GLFWwindow* window;

void init();
void terminate();
void processInput();

int main(){
  init();

  std::string path = "f.obj";

  std::vector<objLoader::Vec4> Vertices;
  std::vector<objLoader::Vec3> TexCoords;
  std::vector<objLoader::Vec4> Normals;
  std::vector<objLoader::Face> Positions;
  std::vector<objLoader::Face> TexPositions;
  std::vector<objLoader::Face> NormPositions;

  if (!objLoader::loadModel(path, Vertices, TexCoords, Normals,
                          Positions, NormPositions, TexPositions)) {
    std::cout << "FAILED TO LOAD OBJ FILE\n";
  }
  std::vector<unsigned int> indices;
  indices.reserve(Positions.size() * 3);

  for(const objLoader::Face& f : Positions) {
    indices.push_back(f.x - 1);
    indices.push_back(f.y - 1);
    indices.push_back(f.z - 1);
  }

  unsigned int VAO, VBO, EBO;
  
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, Vertices.size()*sizeof(objLoader::Vec4), Vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(objLoader::Vec4), (void*)0);
  glEnableVertexAttribArray(0);

  Shader shader("src/vs.glsl", "src/fs.glsl");

  //main loop
  while(!glfwWindowShouldClose(window)){
    processInput();
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(shader.ID);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  terminate();
  return 0;
}
//Main end

void init(){
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Object Loader", NULL, NULL);
  glfwMakeContextCurrent(window);
  glewInit();

  glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
}

void terminate(){
  


  glfwTerminate();
}

void processInput(){
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}
