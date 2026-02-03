#pragma once

#include <GL/glew.h>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

struct Shader{
  unsigned int ID;
  
  Shader(const char* vertexPath, const char* framgentPath){
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderF;
    std::ifstream fShaderF;
        
    vShaderF.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderF.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    
    vShaderF.open(vertexPath);
    fShaderF.open(framgentPath);
    
    //if(!vShaderF || !fShaderF) std::cout<<"ERROR: Couldnt open Shader file\n";

    std::stringstream vShaderS, fShaderS;

    vShaderS << vShaderF.rdbuf();
    fShaderS << fShaderF.rdbuf();

    vShaderF.close();
    fShaderF.close();

    vertexCode = vShaderS.str();
    fragmentCode = fShaderS.str();

    const char* vShader = vertexCode.c_str();  
    const char* fShader = fragmentCode.c_str();
    unsigned int vertex, fragment;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShader, NULL);
    glCompileShader(vertex);
    
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShader, NULL);
    glCompileShader(fragment);

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
  }
};
