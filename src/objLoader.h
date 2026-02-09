#pragma once 

#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
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

struct Mesh{
  std::vector<float> vertices;
  std::vector<float> texCoords;
  std::vector<float> normals;
  std::vector<unsigned int> positions;
  std::vector<unsigned int> texPositions;
  std::vector<unsigned int> normPositions;
  std::string mtl;

  Mesh(std::vector<Face> pos, std::vector<Face> texPos, std::vector<Face> normPos, 
      std::vector<Vec3> Ver, std::vector<Vec3> TexCoords, std::vector<Vec3> Normals, std::string mtl) : mtl(mtl){
    vertices.reserve(pos.size() * 3);
    for(const Vec3& v : Ver) {
      vertices.push_back(v.x);
      vertices.push_back(v.y);
      vertices.push_back(v.z);
    }
    normals.reserve(Normals.size() * 3);
    for(const Vec3& v : Normals) {
      normals.push_back(v.x);
      normals.push_back(v.y);
      normals.push_back(v.z);
    }
    texCoords.reserve(pos.size() * 3);
    for(const Vec3& v : TexCoords) {
      texCoords.push_back(v.x);
      texCoords.push_back(v.y);
      texCoords.push_back(v.z);
    }
    positions.reserve(pos.size() * 3);
    for(const Face& f : pos) {
      positions.push_back(f.x - 1);
      positions.push_back(f.y - 1);
      positions.push_back(f.z - 1);
    }
    texPositions.reserve(pos.size() * 3);
    for(const Face& f : pos) {
      texPositions.push_back(f.x - 1);
      texPositions.push_back(f.y - 1);
      texPositions.push_back(f.z - 1);
    }
    normPositions.reserve(pos.size() * 3);
    for(const Face& f : pos) {
      normPositions.push_back(f.x - 1);
      normPositions.push_back(f.y - 1);
      normPositions.push_back(f.z - 1);
    }
  }

};

struct Object{
  std::string name;
  std::vector<Mesh> meshes;
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

bool loadModel(std::vector<Object>& Objects, std::string path){
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
  std::vector<Vec3> GeometryV;
  std::vector<Vec3> TextureV;
  std::vector<Vec3> NormalV;
  std::vector<Face> Positions;
  std::vector<Face> TPositions;
  std::vector<Face> NPositions; //all define current mesh
  Object currentObj; //stores all meshes
  bool firstObj = true;
  std::string mtl;

  while(std::getline(file, line)){
    std::istringstream iss(line);
    std::vector<std::string> tokens;
    std::string token;

    while(iss >> token)
      tokens.push_back(token);

    if(tokens.empty() || tokens[0][0] == '#')
      continue;

    std::string op = tokens[0];
    
    if(op == "v"){
      if(tokens.size()<4){
        std::cout<<"not enough arguments at line "<<li<<".\n";
        return false;
      }
      else
        GeometryV.push_back(Vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]))); //no support for w component
    }
    //------------------
    else if(op == "vt"){
      if(tokens.size()<2){
        std::cout<<"Error: Not enough arguments at line "<<li<<".\n";
        return false;
      }
      else if(tokens.size() == 2)
        TextureV.push_back(Vec3(std::stof(tokens[1]), 0.0, 0.0));
      else if(tokens.size() == 3)
        TextureV.push_back(Vec3(std::stof(tokens[1]), std::stof(tokens[2]), 1.0));
      else 
        TextureV.push_back(Vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])));
    }
    //------------------
    else if(op == "vn"){
      if(tokens.size()<4){
        std::cout<<"Error: Not enough arguments at line "<<li<<".\n";
        return false;
      }
      NormalV.push_back(Vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])));
    }
    //------------------
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
        Positions.push_back(Face(v[0],v[1],v[2]));
      if(vte)
        TPositions.push_back(Face(vt[0],vt[1],vt[2]));
      if(vne)
        NPositions.push_back(Face(vn[0],vn[1],vn[2]));
    } //support only for triangular faces for now
    //------------------
    else if(op == "mtllib"){
      //load new mtl file 

    }
    //------------------
    else if(op == "usemtl"){
      if(!Positions.empty()){
        currentObj.meshes.push_back(Mesh(Positions, TPositions, NPositions, GeometryV, TextureV, NormalV, mtl)); 
        Positions.clear();
        TPositions.clear();
        NPositions.clear();
        GeometryV.clear();
        TextureV.clear();
        NormalV.clear();
      }
      mtl = tokens[1];
    }
    //------------------
    else if(op == "o"){
      if(firstObj){
        currentObj.name = token[1];
        firstObj=false;
      }
      else{
        currentObj.meshes.push_back(Mesh(Positions, TPositions, NPositions, GeometryV, TextureV, NormalV, mtl));
        Objects.push_back(currentObj);
        currentObj = Object{};
        Positions.clear();
        TPositions.clear();
        NPositions.clear();
        GeometryV.clear();
        TextureV.clear();
        NormalV.clear();
      }
    }
    //------------------
    else if(op == "g"){
      currentObj.meshes.push_back(Mesh(Positions, TPositions, NPositions, GeometryV, TextureV, NormalV, mtl)); //no distinction for groups for now
      Positions.clear();
      TPositions.clear();
      NPositions.clear();
      GeometryV.clear();
      TextureV.clear();
      NormalV.clear();
    }
  }   

  
  return true;
}

}//close namespace
