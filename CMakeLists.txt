####################
# A simple CMake Project File for your reference
# Modify whatever you want
# You can also use other project frameworks
####################

cmake_minimum_required(VERSION 3.18)

project(RenderEngine)
set (CMAKE_CXX_STANDARD 20)

# It is recommended to use vcpkg to install these package
# visit https://github.com/Microsoft/vcpkg for more information
find_package(assimp CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED) # dependencies of tinygltf

find_path(TINYGLTF_INCLUDE_DIRS "tiny_gltf.h")

#add_library(libmain 
#)

add_library(libdeps INTERFACE)
target_link_libraries(libdeps INTERFACE
    Eigen3::Eigen
    glfw
    glm::glm
    glad::glad
    nlohmann_json::nlohmann_json
    assimp::assimp
    libmain
)



# The main opengl framework to of rendering
add_executable(main test/main.cpp "include/shader/shader.h" "include/camera/camera.h" "include/model/mesh.h" "include/model/model.h""include/light/dir_light.h" "include/light/light.h")
#add_dependencies(main libmain)
target_link_libraries(main PRIVATE libdeps)

