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

struct Index {
  Face v;
  Face vt;
  Face vn;
  Index(Face v, Face vt, Face vn) : v(v), vt(vt), vn(vn){}
};

struct Mesh{
  std::vector<unsigned int> positions;
  std::vector<unsigned int> texPositions;
  std::vector<unsigned int> normPositions;
  std::string mtl;

  Mesh(std::vector<Index> pos, std::string mtl, unsigned int n, unsigned int tn, unsigned int nn) : mtl(mtl){
    positions.reserve(pos.size()*3);
    for(const Index x : pos){
      positions.push_back(x.v.x - 1 - n);
      positions.push_back(x.v.y - 1 - n);
      positions.push_back(x.v.z - 1 - n);
    }
    texPositions.reserve(pos.size()*3);
    for(const Index x : pos){
      texPositions.push_back(x.vt.x - 1 - tn);
      texPositions.push_back(x.vt.y - 1 - tn);
      texPositions.push_back(x.vt.z - 1 - tn);
    }
    normPositions.reserve(pos.size()*3);
    for(const Index x : pos){
      normPositions.push_back(x.vn.x - 1 - nn);
      normPositions.push_back(x.vn.y - 1 - nn);
      normPositions.push_back(x.vn.z - 1 - nn);
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
    DiffuseMap="d.png";
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

void parseFragments(std::vector<Index>& Pos, std::vector<std::string> tokens){
  std::vector<int> v, vt, vn;
  for(int i=1; i<tokens.size(); i++){
    std::vector<std::string> parts;
    parseString(parts, tokens[i], "/");
    v.push_back(std::stoi(parts[0]));
    if(parts.size() > 1 && !parts[1].empty())
      vt.push_back(std::stoi(parts[1]));
    else vt.push_back(0);
    if(parts.size() > 2 && !parts[2].empty())
      vn.push_back(std::stoi(parts[2]));
    else vn.push_back(0);
    parts.clear();
  } 

  for(int i=0; i<v.size()-2; i++){
    Face g(v[0], v[i+1], v[i+2]);
    Face t(0,0,0);
    Face n(0,0,0);
    if(!vt.empty()){
      t.x = vt[0]; t.y=vt[i+1]; t.z = vt[i+2];
    }
    if(!vn.empty()){
      n.x = vn[0]; n.y=vn[i+1]; n.z = vn[i+2];
    }
    Pos.push_back(Index(g,t,n));
  }
  v.clear(); vt.clear(); vn.clear();
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
  std::vector<Index> Positions; 
  std::vector<Mesh> Meshes;
  unsigned int vcount = 0, vtcount = 0, vncount = 0;

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
      parseFragments(Positions, tokens);
    }
    //------------------
    else if(op == "mtllib"){
      loadMtl(tokens[1], Materials);
    }
    //------------------
    else if(op == "usemtl"){
      if (!Positions.empty()){
        Meshes.push_back(Mesh(Positions, mtl, vcount, vtcount, vncount)); 
        
        Positions.clear();
      }
      mtl = tokens[1];
    }
    //------------------
    else if(op == "o"){
      if(firstObj){
        objName = tokens[1];
        firstObj=false;
      }
      else{
        Meshes.push_back(Mesh(Positions, mtl, vcount, vtcount, vncount)); 
        
        Objects.push_back(Object(objName, GeometryV, TextureV, NormalV, Meshes));
      
        Positions.clear();
        vcount += GeometryV.size();
        vtcount += TextureV.size();
        vncount += NormalV.size();
        GeometryV.clear();
        TextureV.clear();
        NormalV.clear();
        Meshes.clear();

        objName = tokens[1];
      }
    }
    //------------------
    else if(op == "g"){
      Meshes.push_back(Mesh(Positions, mtl, vcount, vtcount, vncount)); 
        
      Positions.clear();
    }
    li++;
  }   
  
  if(!Positions.empty()){
    Meshes.push_back(Mesh(Positions, mtl, vcount, vtcount, vncount)); 
    Objects.push_back(Object(objName, GeometryV, TextureV, NormalV, Meshes));
  }

  return true;
}

}//close namespace
