#pragma once

#include <GL/glew.h>
#include "glm/ext/vector_float3.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

enum CamDir{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

struct Camera{
  glm::vec3 Pos;
  glm::vec3 Front;
  glm::vec3 Right;
  glm::vec3 Up;
  glm::vec3 WorldUp;
    
  float Yaw;
  float Pitch;

  Camera(glm::vec3 Pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f)) : Pos(Pos), Front(Front), Up(Up){
    WorldUp=Up;
    Yaw = -90.0f;
    Pitch = 0.0f;
    updateCameraVectors();
  };

  glm::mat4 GetViewMatrix(){
    return glm::lookAt(Pos, Pos + Front, Up);
  }

  void ProcessKeyboard(CamDir direction, float deltaTime){
    float velocity = 7.5f * deltaTime;
    
    if (direction == FORWARD)
      Pos += Front * velocity;
    if (direction == BACKWARD)
      Pos -= Front * velocity;
    if (direction == LEFT)
      Pos -= Right * velocity;
    if (direction == RIGHT)
      Pos += Right * velocity;
    if (direction == UP)
      Pos += Up * velocity;
    if (direction == DOWN)
      Pos -= Up * velocity;
  }

  void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true){
    xoffset *= 0.1f;
    yoffset *= 0.1f;

    Yaw += xoffset;
    Pitch += yoffset;

    if (constrainPitch){
      if (Pitch > 89.0f)
        Pitch = 89.0f;
      if (Pitch < -89.0f)
        Pitch = -89.0f;
    }

    updateCameraVectors();
  }

  void updateCameraVectors(){
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    Right = glm::normalize(glm::cross(Front, WorldUp));  
    Up = glm::normalize(glm::cross(Right, Front));
  }

};
