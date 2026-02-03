#pragma once 

#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

namespace objLoader {

struct Vec4{
  float x,y,z,w;
  Vec4(float x, float y, float z, float w=1.0){
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
  }
};

struct Vec3{
  float x,y,z;
  Vec3(float x, float y, float z){
    this->x = x;
    this->y = y;
    this->z = z;
  }
};

struct Face{
  unsigned int x,y,z;
  Face(unsigned int x,unsigned int y,unsigned int z) : x(x), y(y), z(z){}
};

void parseString(std::vector<std::string>& tokens, const std::string& line, const std::string& delimiter){
  int start=0;
  int end;

  while ((end = line.find(delimiter, start)) != std::string::npos) {
    tokens.push_back(line.substr(start, end - start));
    start = end + delimiter.length();
  }

  if (start < line.length())
    tokens.push_back(line.substr(start));
}

bool loadModel(std::string path, std::vector<Vec4>& vertices, std::vector<Vec3>& texCoords, std::vector<Vec4>& normals,
              std::vector<Face>& positions, std::vector<Face>& normPositions, std::vector<Face>& texPositions){
  if(path.substr(path.size()-4,4) != ".obj") return false;

  std::ifstream file(path);
  if(!file.is_open()) return false;

  /*
   * v - vertices
   * vt - texCoords
   * vn - normals
   * vp - parameter space vertices
   * f - polygonal face element
   * l - line element
   * mtl etc. - material
  */

  std::string line;
  uint64_t li = 0;

  while(std::getline(file, line)){
    std::istringstream iss(line);
    std::vector<std::string> tokens;
    std::string token;

    while(iss >> token)
      tokens.push_back(token);

    if(tokens.empty()) 
      continue;
    
    std::string op = tokens[0];
    if (tokens[0][0] == '#')
      continue;

    if(op == "v"){
      if(tokens.size()<4){
        std::cout<<"not enough arguments at line "<<li<<".\n";
        return false;
      }
      else if(tokens.size() == 4)
        vertices.push_back(Vec4(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])));
      else 
        vertices.push_back(Vec4(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]), std::stof(tokens[4])));
    }

    else if(op == "vt"){
      if(tokens.size()<3){
        std::cout<<"Error: Not enough arguments at line "<<li<<".\n";
        return false;
      }
      else if(tokens.size() == 3)
        texCoords.push_back(Vec3(std::stof(tokens[1]), std::stof(tokens[2]), 1.0));
      else 
        texCoords.push_back(Vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])));
    }
    else if(op == "vn"){
      if(tokens.size()<4){
        std::cout<<"Error: Not enough arguments at line "<<li<<".\n";
        return false;
      }
      normals.push_back(Vec4(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])));
    }
    else if(op == "f"){
      if(tokens.size()<4){
        std::cout<<"Error: Not enough arguments at line "<<li<<".\n";
        return false;
      }

      unsigned int v[3]; 
      unsigned int vt[3]; 
      unsigned int vn[3]; 
      bool ve=false, vte=false, vne=false;

      for(int i=1;i<4;i++){
        std::vector<std::string> parts;
        parseString(parts, tokens[i], "/");

        v[i-1] = std::stoi(parts[0]);
        
        if (parts.size() >= 1 && !parts[0].empty()){
          v[i-1] = std::stoi(parts[0]);
          ve = true;
        }

        if (parts.size() >= 2 && !parts[1].empty()){
          vt[i-1] = std::stoi(parts[1]);
          vte=true;
        }

        if (parts.size() >= 3 && !parts[2].empty()){
          vn[i-1] = std::stoi(parts[2]);
          vne=true;
        }
      }
      if(ve)
        positions.push_back(Face(v[0],v[1],v[2]));
      if(vte)
        texPositions.push_back(Face(vt[0],vt[1],vt[2]));
      if(vne)
        normPositions.push_back(Face(vn[0],vn[1],vn[2]));
    }
  }   

  
  return true;
}

}//close namespace
