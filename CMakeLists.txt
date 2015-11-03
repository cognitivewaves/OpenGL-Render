cmake_minimum_required (VERSION 2.8)

project (OpenGL-Render)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/_install)

###################################
#     configure freeglut library  #
###################################
message("Configuring freeglut library...")
add_subdirectory(freeglut-3.0.0)
message("Configuring freeglut library - done")

###################################
#         configure executable    #
###################################
find_package(OpenGL REQUIRED)
if (NOT OPENGL_GLU_FOUND)
    message(FATAL_ERROR "OpenGL Utility - GLU library not found")
endif (NOT OPENGL_GLU_FOUND)

include_directories(freeglut-3.0.0/include)

add_executable (app main.cpp)
target_link_libraries (app freeglut ${OPENGL_glu_LIBRARY})

install (TARGETS app RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

