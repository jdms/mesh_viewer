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

# The following is only necessary if building 
# tucanow as a dll in windows

set_target_properties(tucanow PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

set_target_properties(tucanow PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)


###############################################
# Making glfw3 work is more fun!
###############################################

# if(WIN32)
    # add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/win32-libs")

# else()
    # As per its docs, glfw3 requires the use of 
    # pkg-config (see `man pkg-config`)

    # find_package(PkgConfig REQUIRED)
    find_package(glfw3 REQUIRED)

    # pkg_search_module(GLFW REQUIRED glfw3)
    # include_directories(${GLFW_INCLUDE_DIRS})

    # What do we have, static or dynamic libraries?
    # if(NOT GLFW_LIBRARIES)
        # set(GLFW_LIBRARIES ${GLFW_STATIC_LIBRARIES})
    # endif()
    # link_libraries(${GLFW_LIBRARIES})
# endif()


###############################################
# Build binary
###############################################

# Don't touch CMAKE_CXX_FLAGS!
# set(CMAKE_CXX_FLAGS 
#     "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -ggdb3")
add_library(twindow
    ${CMAKE_SOURCE_DIR}/src/twindow.cpp
)

target_compile_features(twindow PUBLIC cxx_std_11)

target_include_directories(twindow 
    PUBLIC 
        "${CMAKE_SOURCE_DIR}/include"
    # PRIVATE 
        # "${GLFW_INCLUDE_DIRS}"
)

target_link_libraries(twindow 
    PUBLIC 
        tucanow
    PRIVATE
        glfw
)

add_executable(main_spheres
    ${CMAKE_SOURCE_DIR}/src/main_spheres.cpp
)

target_link_libraries(main_spheres twindow)

if (glfw3_VERSION VERSION_GREATER_EQUAL 3.3)
    target_compile_options(twindow PRIVATE -DTWINDOW_GLFW3_VERSION_3_3_OR_GREATER)
    message(STATUS "Enabling glfw3 native window content scaling")
endif()

add_executable(main_ply
    ${CMAKE_SOURCE_DIR}/src/main_ply.cpp
)

target_link_libraries(main_ply twindow)

###############################################
# Copy models to binary dir when done
###############################################

add_custom_command(TARGET twindow POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/samples/models
        ${CMAKE_CURRENT_BINARY_DIR}/samples/models
)
