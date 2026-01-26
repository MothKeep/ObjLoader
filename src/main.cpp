#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "objLoader.h"

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

  std::cout<<Vertices.size()<<"\n";
  std::cout<<TexCoords.size()<<"\n";
  std::cout<<Normals.size()<<"\n";
  std::cout<<Positions.size()<<"\n";
  std::cout<<TexPositions.size()<<"\n";
  std::cout<<NormPositions.size()<<"\n";


  while(!glfwWindowShouldClose(window)){
    processInput();
    glfwSwapBuffers(window);
    glfwPollEvents();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
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
