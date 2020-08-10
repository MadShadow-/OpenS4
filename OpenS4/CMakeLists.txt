
cmake_minimum_required (VERSION 3.8)


file(GLOB_RECURSE src "${PROJECT_SOURCE_DIR}" "*.cpp")
file(GLOB_RECURSE header "${PROJECT_SOURCE_DIR}" "*.hpp")

add_executable(OpenS4 ${src} ${header})
#add_executable(OpenS4 main.cpp "Renderer/TextureAtlas.cpp" "Renderer/LandscapeTextures.cpp"  "Renderer/Shaders.hpp" "Renderer/Shaders.cpp"  "Renderer/TriangleBatch.hpp" "Renderer/LandscapeRenderer.hpp" "Renderer/MatrixStack.hpp" "Renderer/TransformationPipeline.hpp" "Renderer/LoadShader.hpp" "Import/Gfx.cpp" "Import/Reader.cpp"  "Import/MapReader/Map.cpp" "Import/MapReader/Map.hpp" "Import/MapReader/Reader.cpp" "Import/MapReader/Reader.hpp")
set_property(TARGET OpenS4 PROPERTY CXX_STANDARD 17)


include_directories("../Dependencies/glew-2.1.0/include")
include_directories("../Dependencies/glfw-3.2.2/include")

include_directories("../Dependencies")


target_link_libraries(OpenS4 glew)
target_link_libraries(OpenS4 glfw)
target_link_libraries(OpenS4 imgui_lib)

target_link_libraries(OpenS4 tinyxml2)

#set(GLFW_INCLUDE_DIR "../Dependencies/GLFW/include" CACHE PATH "GLFW_INCLUDE_DIR")
#set(GLFW_LIBRARY "../Dependencies/GLFW/glfw3.lib" CACHE FILEPATH "GLFW_LIBRARY")
#include_directories(${GLFW_INCLUDE_DIR})
#add_library(glfw STATIC IMPORTED)
#set_target_properties( glfw PROPERTIES IMPORTED_LOCATION ${GLFW_LIBRARY} )
#target_link_libraries(OpenS4 glfw)


#set(GLEW_INCLUDE_DIR "../Dependencies/GLEW/include" CACHE PATH "GLEW_INCLUDE_DIR")
#set(GLEW_LIBRARY "../Dependencies/GLEW/glew32s.lib" CACHE FILEPATH "GLEW_LIBRARY")
#include_directories(${GLEW_INCLUDE_DIR})
#add_library(glew STATIC IMPORTED)
#set_target_properties( glew PROPERTIES IMPORTED_LOCATION ${GLEW_LIBRARY} )
#target_link_libraries(OpenS4 glew)

include_directories(".")


include_directories(${glm_INCLUDE_DIR})
#include_directories(${GLTools_INCLUDE_DIR})

include_directories("../Dependencies/rectpack2D/include")
include_directories("../Dependencies/imgui")


# Copy JSON configuration data
configure_file(${PROJECT_SOURCE_DIR}/Config/Buildings/romanBuildingsBaseFrame.json ${CMAKE_CURRENT_BINARY_DIR}/Config/Buildings/romanBuildingsBaseFrame.json COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/Config/Buildings/romanBuildings.json ${CMAKE_CURRENT_BINARY_DIR}/Config/Buildings/romanBuildings.json COPYONLY)

configure_file(${PROJECT_SOURCE_DIR}/Config/Object/ObjectImport.json ${CMAKE_CURRENT_BINARY_DIR}/Config/Object/ObjectImport.json COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/Config/Object/ObjectAnimations.json ${CMAKE_CURRENT_BINARY_DIR}/Config/Object/ObjectAnimations.json COPYONLY)

# Copy Maps
configure_file(${PROJECT_SOURCE_DIR}/Config/Maps/576_AlleTexturen.map ${CMAKE_CURRENT_BINARY_DIR}/Config/Maps/576_AlleTexturen.map COPYONLY)
