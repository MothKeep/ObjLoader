#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "glm/detail/qualifier.hpp"
#include "glm/ext/vector_float3.hpp"
#include "objLoader.h"
#include "shader.h"
#include "camera.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#define SCR_WIDTH 1920
#define SCR_HEIGHT 1080

Camera cam(glm::vec3(0.0f, 0.0f, -3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;	
float lastFrame = 0.0f;

void init(GLFWwindow*& window);
void terminate();
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);

int main(){
  GLFWwindow* window;
  init(window);

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
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;   processInput(window);
    
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(shader.ID);
  
    glm::mat4 view = glm::mat4(1.0f); 
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    view = cam.GetViewMatrix();

    
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, &view[0][0]);
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  terminate();
  return 0;
}
//Main end

void init(GLFWwindow*& window){
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Object Loader", NULL, NULL);
  glfwMakeContextCurrent(window);
  glewInit();

  glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_callback);

  glEnable(GL_DEPTH_TEST);
}

void terminate(){
  


  glfwTerminate();
}

void processInput(GLFWwindow* window){
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  
  if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    cam.ProcessKeyboard(FORWARD, deltaTime);
  if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    cam.ProcessKeyboard(BACKWARD, deltaTime);
  if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    cam.ProcessKeyboard(LEFT, deltaTime);
  if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    cam.ProcessKeyboard(RIGHT, deltaTime);
  if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    cam.ProcessKeyboard(UP, deltaTime);
  if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    cam.ProcessKeyboard(DOWN, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn){
  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);

  if(firstMouse){
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;

  lastX = xpos;
  lastY = ypos;

  cam.ProcessMouseMovement(xoffset, yoffset);
}
