
cmake_minimum_required (VERSION 3.8)


file(GLOB_RECURSE src "${PROJECT_SOURCE_DIR}" "*.cpp")
file(GLOB_RECURSE header "${PROJECT_SOURCE_DIR}" "*.hpp")

add_executable(OpenS4 ${src} ${header})
#add_executable(OpenS4 main.cpp "Renderer/TextureAtlas.cpp" "Renderer/LandscapeTextures.cpp"  "Renderer/Shaders.hpp" "Renderer/Shaders.cpp"  "Renderer/TriangleBatch.hpp" "Renderer/LandscapeRenderer.hpp" "Renderer/MatrixStack.hpp" "Renderer/TransformationPipeline.hpp" "Renderer/LoadShader.hpp" "Import/Gfx.cpp" "Import/Reader.cpp"  "Import/MapReader/Map.cpp" "Import/MapReader/Map.hpp" "Import/MapReader/Reader.cpp" "Import/MapReader/Reader.hpp")
set_property(TARGET OpenS4 PROPERTY CXX_STANDARD 17)


include_directories("../Dependencies/glew-2.1.0/include")
include_directories("../Dependencies/glfw-3.2.2/include")


target_link_libraries(OpenS4 glew)
target_link_libraries(OpenS4 glfw)


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



#file (GLOB_RECURSE shaders "shaders/*.*")
#foreach(resource ${shaders})
# get_filename_component(filename ${resource} NAME)
# get_filename_component(dir ${resource} DIRECTORY)
# get_filename_component(dirname ${dir} NAME)
 
# set (output "")
 
# while(NOT ${dirname} STREQUAL resources)
#  get_filename_component(path_component ${dir} NAME)
#  set (output "${path_component}/${output}")
#  get_filename_component(dir ${dir} DIRECTORY)
#  get_filename_component(dirname ${dir} NAME)
# endwhile()
 
# set(output "${CMAKE_CURRENT_BINARY_DIR}/shaders/${filename}")
# 
# add_custom_command(
# COMMENT "Moving updated resource-file '${filename}' '${output}"
#  OUTPUT ${output}
# DEPENDS ${resource}
# COMMAND ${CMAKE_COMMAND} -E copy_if_different
# ${resource}
# ${output}
#)
#add_custom_target(${filename} ALL DEPENDS ${resource} ${output})
#
#endforeach()
