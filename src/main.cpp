#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <complex>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include "Renderer.h"
#include "glm/detail/qualifier.hpp"
#include "glm/ext/vector_float3.hpp"
#include "objLoader.h"
#include "shader.h"
#include "camera.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#define SCR_WIDTH 1920
#define SCR_HEIGHT 1080

Camera cam(glm::vec3(20.0f, 0.0f, 20.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;	
float lastFrame = 0.0f;

void init(GLFWwindow*& window, bool flip);
void terminate();
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);

int main(int32_t _argc, char** _argv){
  if (_argc < 2) {
    std::cout<<"Missing file path\nDo: ./objLoader <filepath> <texture flip (0|1)*> <number of lights*>\n";
		return EXIT_FAILURE;
	}
	if (!std::filesystem::exists(_argv[1])) {
    std::cout<<"Couldn't find requested file. Closing\n";
		return EXIT_FAILURE;
	}
  std::string path = _argv[1];
  
  bool flip = true;
  if(_argc > 2 && std::strcmp(_argv[2], "0") == 0) {
    flip = false;
  }

  int lights = 3;
  if(_argc > 3) lights=std::stoi(_argv[3]);

  lights = (lights > 50) ? 50 : lights; 
  GLFWwindow* window;
  init(window, flip);


  std::vector<objLoader::Object> Objects;
  std::vector<objLoader::Material> Materials;
  if (!objLoader::loadObject(Objects, Materials, path) ) {
    std::cout << "FAILED TO LOAD OBJ FILE\n";
  }

  Shader shader("src/vs.glsl", "src/fs.glsl");

  std::vector<Renderer::Model> ObjModels;
  for(int i=0; i<Objects.size(); i++) 
    ObjModels.push_back(Renderer::LoadObject(Objects[i], Materials));

  glUseProgram(shader.ID);
  GLint SetProj = glGetUniformLocation(shader.ID, "projection");
  GLint SetView = glGetUniformLocation(shader.ID, "view");
  
  GLint SetMesh[6];
  SetMesh[0] = glGetUniformLocation(shader.ID, "material.ambient");
  SetMesh[1] = glGetUniformLocation(shader.ID, "material.diffuse");
  SetMesh[2] = glGetUniformLocation(shader.ID, "material.specular");
  SetMesh[3] = glGetUniformLocation(shader.ID, "material.shininess");
  SetMesh[4] = glGetUniformLocation(shader.ID, "material.diffuseM"); 
  SetMesh[5] = glGetUniformLocation(shader.ID, "state"); 
        
/* 
  SetLight[0] = glGetUniformLocation(shader.ID, "light.position");
  SetLight[1] = glGetUniformLocation(shader.ID, "light.ambient");
  SetLight[2]= glGetUniformLocation(shader.ID, "light.diffuse");
  SetLight[3]= glGetUniformLocation(shader.ID, "light.specular");
  SetLight[4]= glGetUniformLocation(shader.ID, "Nr_Lights");
  */  
  glUniform3f(glGetUniformLocation(shader.ID, "light.ambient"), 0.2f, 0.2f, 0.2f); 
  glUniform3f(glGetUniformLocation(shader.ID, "light.diffuse"), 0.8f, 0.8f, 0.8f); 
  glUniform3f(glGetUniformLocation(shader.ID, "light.specular"), 1.0f, 1.0f, 1.0f); 
  
  glUniform1i(glGetUniformLocation(shader.ID, "Nr_Lights"), lights); 
  float offset = 17.0f;
  float radius = 24.0f;
  for (unsigned int i = 0; i < lights; i++){
    float angle = (float)i / (float)lights * 360.0f;
    float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
    float x = sin(angle) * radius + displacement;
    displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
    float y = displacement * 0.4f + 20.0f;  
    displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
    float z = cos(angle) * radius + displacement;
    std::string name = "Lights[" + std::to_string(i) + "].position";
    glUniform3f(glGetUniformLocation(shader.ID, name.c_str()), x, y, z); 
  }
  GLint SetPos = glGetUniformLocation(shader.ID, "viewPos");

  glm::mat4 model = glm::mat4(1.0f); 
  glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, &model[0][0]);
  
  //main loop
  while(!glfwWindowShouldClose(window)){
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;   processInput(window);
    
    glClearColor(0.06f, 0.06f, 0.06f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(shader.ID);
    glm::mat4 view = glm::mat4(1.0f); 
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(50.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    view = cam.GetViewMatrix();
    
    glUniform3fv(SetPos, 1, &cam.Pos[0]); 
    glUniformMatrix4fv(SetProj, 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(SetView, 1, GL_FALSE, &view[0][0]);

    for(auto& objMod : ObjModels)
      Renderer::RenderObject(objMod, shader, SetMesh, Materials);
    
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  for(auto& objMod : ObjModels)
    Renderer::DestroyModel(objMod);
  
  terminate();
  return 0;
}
//Main end

void init(GLFWwindow*& window, bool flip){
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

  stbi_set_flip_vertically_on_load(flip);
  
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
