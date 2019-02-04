###############################################
# Preamble  
###############################################

cmake_minimum_required(VERSION 3.0)

project(MeshViewer)

# For vim's YouCompleteMe plugin
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


###############################################
# External libraries
###############################################

add_subdirectory(${CMAKE_SOURCE_DIR}/src/tucanow)


###############################################
# Making glfw3 work is more fun! 
###############################################

# As per its docs, glfw3 requires the use of 
# pkg-config (see `man pkg-config`)

find_package(PkgConfig REQUIRED)

pkg_search_module(GLFW REQUIRED glfw3)
# include_directories(${GLFW_INCLUDE_DIRS})

# What do we have, static or dynamic libraries?
if(NOT GLFW_LIBRARIES)
    set(GLFW_LIBRARIES ${GLFW_STATIC_LIBRARIES})
endif()
# link_libraries(${GLFW_LIBRARIES})


###############################################
# Build binary
###############################################

# Don't touch CMAKE_CXX_FLAGS!
# set(CMAKE_CXX_FLAGS 
#     "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -ggdb3") 

add_executable(mesh_viewer
    ${CMAKE_SOURCE_DIR}/src/main_window.cpp
    ${CMAKE_SOURCE_DIR}/src/main.cpp
)
target_compile_features(mesh_viewer PUBLIC cxx_std_11)
target_include_directories(mesh_viewer 
    PUBLIC "${CMAKE_SOURCE_DIR}/include"
    PRIVATE "${GLFW_INCLUDE_DIRS}"
)
target_link_libraries(mesh_viewer ${GLFW_LIBRARIES} tucanow::tucanow)

###############################################
# Copy models to binary dir when done
###############################################

add_custom_command(TARGET mesh_viewer POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/samples/models
        ${CMAKE_CURRENT_BINARY_DIR}/samples/models
)
