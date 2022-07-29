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

set(SOURCES
src/scene/scene.cpp
src/model/model.cpp
src/utils/SH.cpp
 "src/material/texture.cpp" "src/EnvGenerator/EnvGenerator.cpp")



# The main opengl framework to of rendering
add_executable(main "test/test_PCSS.cpp" ${SOURCES})
#add_dependencies(main libmain)
target_link_libraries(main 
PRIVATE libdeps
${PROJECT_SOURCE_DIR}/include
)
add_executable(test_PRT test/test_PRT.cpp ${SOURCES} )
target_link_libraries(test_PRT 
PRIVATE libdeps
${PROJECT_SOURCE_DIR}/include
)


add_executable(test_Kulla_Conty test/test_Kulla_Conty.cpp ${SOURCES})
target_link_libraries(test_Kulla_Conty 
PRIVATE libdeps
${PROJECT_SOURCE_DIR}/include
)

add_executable(test_IBL test/test_IBL.cpp ${SOURCES})
target_link_libraries(test_IBL
PRIVATE libdeps
${PROJECT_SOURCE_DIR}/include
)