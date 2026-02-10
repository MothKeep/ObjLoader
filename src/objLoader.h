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
  std::vector<unsigned int> positions;
  std::vector<unsigned int> texPositions;
  std::vector<unsigned int> normPositions;
  std::string mtl;

  Mesh(std::vector<Face> pos, std::vector<Face> texPos, std::vector<Face> normPos, std::string mtl, unsigned int n,  unsigned int tn,  unsigned int nn) : mtl(mtl){
    positions.reserve(pos.size() * 3);
    for(const Face& f : pos) {
      positions.push_back(f.x - 1 - n);
      positions.push_back(f.y - 1 - n);
      positions.push_back(f.z - 1 - n);
    }
    texPositions.reserve(texPos.size() * 3);
    for(const Face& f : texPos) {
      texPositions.push_back(f.x - 1 - tn);
      texPositions.push_back(f.y - 1 - tn);
      texPositions.push_back(f.z - 1 - tn);
    }
    normPositions.reserve(normPos.size() * 3);
    for(const Face& f : normPos) {
      normPositions.push_back(f.x - 1 - nn);
      normPositions.push_back(f.y - 1 - nn);
      normPositions.push_back(f.z - 1 - nn);
    }
  }

};

struct Object{
  std::vector<float> vertices;
  std::vector<float> texCoords;
  std::vector<float> normals;
  
  std::string name;
  std::vector<Mesh> meshes;
  Object(std::string name, std::vector<Vec3> Vertices, std::vector<Vec3> TexCoords, std::vector<Vec3> Normals, std::vector<Mesh> Meshes) : name(name), meshes(Meshes){
    vertices.reserve(Vertices.size() * 3);
    for(const Vec3 v : Vertices) {
      vertices.push_back(v.x);
      vertices.push_back(v.y);
      vertices.push_back(v.z);
    }
    texCoords.reserve(TexCoords.size() * 3);
    for(const Vec3 v : TexCoords) {
      texCoords.push_back(v.x);
      texCoords.push_back(v.y);
      texCoords.push_back(v.z);
    }
    normals.reserve(Normals.size() * 3);
    for(const Vec3 v : Normals) {
      normals.push_back(v.x);
      normals.push_back(v.y);
      normals.push_back(v.z);
    }
  }
};

struct Material{
  std::string name;
  std::string DiffuseMap;
  std::string AmbientMap;
  std::string SpecularMap;
  std::string BumpMap; //map paths
  float ambient[3];
  float diffuse[3];
  float specular[3]; //colors
  float emission[3]; //emissed light
  float sExponent; //shininess
  float opacity;
  Material(std::string name="") : name(name){
    DiffuseMap="";
    AmbientMap="";
    SpecularMap="";
    BumpMap="";
    ambient[0] = 1.0; ambient[1] = 1.0; ambient[2] = 1.0;
    diffuse[0] = 1.0; diffuse[1] = 1.0; diffuse[2] = 1.0;
    specular[0] = 1.0; specular[1] = 1.0; specular[2] = 1.0;
    sExponent = 1.0;
    opacity = 1.0;
  };
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

void loadMtl(std::string path, std::vector<Material>& Materials){
  std::ifstream mFile(path);
  if(!mFile.is_open()) return;
  
  std::string line;
  Material curMat;
  
  while(std::getline(mFile, line)){
    std::istringstream iss(line);
    std::vector<std::string> tokens;
    std::string token;

    while(iss >> token)
      tokens.push_back(token);

    if(tokens.empty() || tokens[0][0] == '#')
      continue;

    std::string op = tokens[0];
    
    if(op == "newmtl"){
      if(curMat.name != ""){
        Materials.push_back(curMat);
      }
      curMat = Material(tokens[1]);
    }
    //------------------
    else if(op == "Ka"){
      curMat.ambient[0] = std::stof(tokens[1]);   
      curMat.ambient[1] = std::stof(tokens[2]);   
      curMat.ambient[2] = std::stof(tokens[3]);   
    }
    //------------------
    else if(op == "Kd"){
      curMat.diffuse[0] = std::stof(tokens[1]);   
      curMat.diffuse[1] = std::stof(tokens[2]);   
      curMat.diffuse[2] = std::stof(tokens[3]);   
    }
    //------------------
    else if(op == "Ks"){
      curMat.specular[0] = std::stof(tokens[1]);   
      curMat.specular[1] = std::stof(tokens[2]);   
      curMat.specular[2] = std::stof(tokens[3]);   
    }
    //------------------
    else if(op == "Ke"){
      curMat.emission[0] = std::stof(tokens[1]);   
      curMat.emission[1] = std::stof(tokens[2]);   
      curMat.emission[2] = std::stof(tokens[3]);
    }
    //------------------
    else if(op == "Ns")
      curMat.sExponent = std::stof(tokens[1]);
    //------------------
    else if(op == "d" || op == "Tr")
      curMat.opacity = std::stof(tokens[1]);
    //------------------
    else if(op == "map_Ka")
      curMat.AmbientMap = tokens[1];
    //------------------
    else if(op == "map_Kd")
      curMat.DiffuseMap= tokens[1];
    //------------------
    else if(op == "map_Ks")
      curMat.SpecularMap = tokens[1];
    //------------------
    else if(op == "map_Bump" || op == "bump")
      curMat.BumpMap = tokens[1];
  }
  if (curMat.name != "")
    Materials.push_back(curMat);
}

bool loadObject(std::vector<Object>& Objects, std::vector<Material>& Materials, std::string path){
  if(path.substr(path.size()-4,4) != ".obj") return false;

  std::ifstream file(path);
  if(!file.is_open()) return false;

  if(Materials.empty()) 
    Materials.push_back(Material("Default"));

  std::string line;
  uint64_t li = 0;
  std::vector<Vec3> GeometryV;
  std::vector<Vec3> TextureV;
  std::vector<Vec3> NormalV;  //define current obj
  std::vector<Face> Positions;
  std::vector<Face> TPositions;
  std::vector<Face> NPositions; //define current mesh 

  std::vector<Mesh> Meshes;
  unsigned int n=0, tn=0, nn=0;

  bool firstObj = true;
  std::string mtl = "";
  std::string objName = "";

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
      loadMtl(tokens[1], Materials);
    }
    //------------------
    else if(op == "usemtl"){
      if (!Positions.empty()){
        Meshes.push_back(Mesh(Positions, TPositions, NPositions, mtl, n, tn, nn)); 
        n+=Positions.size()*3;
        tn+=TPositions.size()*3;
        nn+=NPositions.size()*3;
        
        Positions.clear();
        TPositions.clear();
        NPositions.clear();
      }
      mtl = tokens[1];
    }
    //------------------
    else if(op == "o"){
      if(firstObj){
        objName = token[1];
        firstObj=false;
      }
      else{
        Meshes.push_back(Mesh(Positions, TPositions, NPositions, mtl, n, tn, nn));
        Objects.push_back(Object(objName, GeometryV, TextureV, NormalV, Meshes));
        n+=Positions.size()*3;
        tn+=TPositions.size()*3;
        nn+=NPositions.size()*3;
        
        Positions.clear();
        TPositions.clear();
        NPositions.clear();
        GeometryV.clear();
        TextureV.clear();
        NormalV.clear();
        Meshes.clear();

        objName = token[1];
      }
    }
    //------------------
    else if(op == "g"){
      Meshes.push_back(Mesh(Positions, TPositions, NPositions, mtl, n, tn, nn)); //no distinction for groups for now
      n+=Positions.size()*3;
      tn+=TPositions.size()*3;
      nn+=NPositions.size()*3;
      
      Positions.clear();
      TPositions.clear();
      NPositions.clear();
    }
  }   
  
  if(!Positions.empty()){
    Meshes.push_back(Mesh(Positions, TPositions, NPositions, mtl, n, tn, nn)); 
    Objects.push_back(Object(objName, GeometryV, TextureV, NormalV, Meshes));
  }

  return true;
}

}//close namespace
