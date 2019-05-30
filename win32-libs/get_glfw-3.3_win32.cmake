###############################################
# Script options
###############################################


###############################################
# Download glew and extract it to GLFW_DIR
###############################################

function(DownloadGLFW3 LIBS_DIR GLFW_DIR)
    # Todo: set an option to allow picking a 32bit lib
    set(GLFW_URL "https://github.com/glfw/glfw/releases/download/3.3/glfw-3.3.bin.WIN64.zip")
    set(GLFW_PATH "${LIBS_DIR}/glfw-3.3.bin.WIN64")
    set(GLFW_ZIP_PATH "${LIBS_DIR}/glfw-3.3.bin.WIN64.zip")

    if (NOT EXISTS "${GLFW_PATH}")
        file(MAKE_DIRECTORY "${LIBS_DIR}")

        message(STATUS "Downloading glfw3...")
        file(DOWNLOAD "${GLFW_URL}" "${GLFW_ZIP_PATH}")

        message(STATUS "Uncompressing glfw3...")
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E tar "xf" "${GLFW_ZIP_PATH}" --format=zip
            WORKING_DIRECTORY ${LIBS_DIR}
        )

    file(REMOVE "${GLFW_ZIP_PATH}")
    endif()

    set(GLFW_DIR ${GLFW_PATH} PARENT_SCOPE)

endfunction(DownloadGLFW3)

macro(GetGLFW3)

    ###############################################
    # Set env vars and download glew
    ###############################################

    if (NOT GLFW_PARENT_DIR)
        set(GLFW_PARENT_DIR "${CMAKE_CURRENT_LIST_DIR}")
    endif()

    DownloadGLFW3(${GLFW_PARENT_DIR} GLFW_DIR)


    ###############################################
    # Find include dir
    ###############################################

    find_path(GLFW_INCLUDE_DIRS GLFW/glfw3.h HINTS ${GLFW_DIR}/include)

    if (GLFW_INCLUDE_DIRS)
        message(STATUS "Found GLFW include dir: ${GLFW_INCLUDE_DIRS}")
    else()
        message(FATAL_ERROR "Could not find GLFW include dir -- abort")
    endif()


    ###############################################
    # Find static library
    ###############################################

    find_library(GLFW_LIBRARIES NAMES glfw3dll HINTS "${GLFW_DIR}/lib-vc2013")
    find_program(GLFW_RUNTIME NAMES glfw3.dll HINTS "${GLFW_DIR}/lib-vc2013")

    if (GLFW_LIBRARIES)
        message(STATUS "Found GLFW library: ${GLFW_LIBRARIES}")
        message(STATUS "Found GLFW runtime: ${GLFW_RUNTIME}")
        set(glfw_VERSION "3.3" PARENT_SCOPE)
        set(glfw_VERSION_MAJOR "3" PARENT_SCOPE)
        set(glfw_VERSION_MINOR "3" PARENT_SCOPE)
    else()
        message(FATAL_ERROR "Could not find GLFW library -- abort")
    endif()


    ###############################################
    # Create library interface
    ###############################################

    if (NOT TARGET glfw) 
        add_library(glfw INTERFACE)
        target_include_directories(glfw INTERFACE ${GLFW_INCLUDE_DIRS})
        target_link_libraries(glfw INTERFACE ${GLFW_LIBRARIES})

        add_library(glfw::glfw3 ALIAS glfw)
    endif()


    ###############################################
    # Copy dll to build dir
    ###############################################

    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${GLFW_RUNTIME} ${CMAKE_BINARY_DIR}
    )


endmacro(GetGLFW3)
