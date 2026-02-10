cmake_minimum_required(VERSION 3.20.0)
set(CMAKE_CXX_STANDARD 20)

project(objLoader)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

set(CMAKE_SOURCE_DIR "${MY_ROOT}/src")
list(APPEND sourcesList 
  "src/main.cpp" 
  #"src/objLoader.h" 
  #"src/shader.h" 
  #"src/stb_image.h"
  "src/stb_image.cpp"
) 
add_executable(${PROJECT_NAME} ${sourcesList})

target_link_libraries(${PROJECT_NAME}
    OpenGL::GL
    glfw
    GLEW::GLEW
)
