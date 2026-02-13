#version 330 core

out vec4 FragColor;

/*
  struct Material{
  std::string name;
  std::string DiffuseMap;
  std::string AmbientMap;
  std::string SpecularMap;
  std::string BumpMap; //map paths
  float ambient[3];
  float diffuse[3];
  float specular[3]; //colors
  float sExponent; //shininess    
 */
struct Material {
  vec3 ambient;
  sampler2D diffuseM;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};

struct Light {
  vec3 position;
  
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};


in vec3 Normal;  
in vec3 FragPos;  
in vec2 TexCoords;

uniform Material material;
uniform Light light;
uniform int state; //0 - vertices  1 - vertices and texture  2 - vertices and normals  3 - all

uniform vec3 viewPos;

void main(){
  vec3 result = vec3(0.0);
  float ambientStrength=0.2;

  //pure vertices
  if(state == 0){ 
    vec3 ambient = ambientStrength * light.ambient * material.ambient;

    result = ambient;
  }
  //vertices + texture
  else if(state == 1){ 
    vec3 ambient = vec3(texture(material.diffuseM, TexCoords)) * ambientStrength * light.ambient * material.ambient; 

    result = ambient;
  }
  //vertices + normals
  else if(state == 2){ 
    vec3 ambient = ambientStrength * light.ambient * material.ambient;

    vec3 lightDir = normalize(light.position - FragPos);
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = (diff * material.diffuse) * light.diffuse;
  
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec*material.specular);

    result = ambient + diffuse + specular;
  }
  //all 
  else if(state == 3){ 
    vec3 ambient = vec3(texture(material.diffuseM, TexCoords)) * ambientStrength * light.ambient * material.ambient;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(texture(material.diffuseM, TexCoords)) * material.diffuse * light.diffuse;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec*material.specular);

    result = ambient + diffuse + specular;
  }

  FragColor = vec4(result, 1.0);
}
