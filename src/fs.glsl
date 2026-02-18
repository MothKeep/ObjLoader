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
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct Position{
  vec3 position;
};

uniform int Nr_Lights;
uniform Position Lights[50];

in vec3 Normal;  
in vec3 FragPos;  
in vec2 TexCoords;

uniform Material material;
uniform Light light;
uniform int state; //0 - vertices  1 - vertices and texture  2 - vertices and normals  3 - all

uniform vec3 viewPos;

void main(){
  vec3 result = vec3(0.0);
  float ambientStrength=0.3;

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
    vec3 norm = normalize(Normal);
    
    float lightFactor = 1.0 / float(Nr_Lights);
    
    result = ambient;
    for(int i=0; i<Nr_Lights; i++){
      //float distance = length(Lights[i].position - FragPos);
      //float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
      
      vec3 lightDir = normalize(Lights[i].position - FragPos);
      float diff = max(dot(norm, lightDir), 0.0);
      vec3 diffuse = (diff * material.diffuse) * light.diffuse;
  
      vec3 viewDir = normalize(viewPos - FragPos);
      vec3 reflectDir = reflect(-lightDir, norm);  
  
      float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
      vec3 specular = light.specular * (spec*material.specular);
      
      //result += (specular * attenuation + diffuse * attenuation) * lightFactor;
      result += (specular + diffuse) * lightFactor;
    }
  }
  //all 
  else if(state == 3){ 
    vec3 ambient = vec3(texture(material.diffuseM, TexCoords)) * ambientStrength * light.ambient * material.ambient;
    vec3 norm = normalize(Normal);
    float lightFactor = 2.0 / float(Nr_Lights);
    
    result = ambient;

    for(int i=0; i<Nr_Lights; i++){
      //float distance = length(Lights[i].position - FragPos);
      //float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
      
      vec3 lightDir = normalize(Lights[i].position - FragPos);
      float diff = max(dot(norm, lightDir), 0.0);
      vec3 diffuse = diff * vec3(texture(material.diffuseM, TexCoords)) * material.diffuse * light.diffuse;
  
      vec3 viewDir = normalize(viewPos - FragPos);
      vec3 reflectDir = reflect(-lightDir, norm);  
  
      float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
      vec3 specular = light.specular * (spec*material.specular);
      
      //result += (specular * attenuation + diffuse * attenuation) * lightFactor;
      result += (specular + diffuse) * lightFactor;
    }
  }

  FragColor = vec4(result, 1.0);
}
