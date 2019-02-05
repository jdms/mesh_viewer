###############################################
# Script options
###############################################

option(GLEW_BUILD_x64 "Use glew x64 library" ON)


###############################################
# Download glew and extract it to GLEW_DIR
###############################################

function(DownloadGLEW LIBS_DIR GLEW_DIR)
    set(GLEW_URL "https://downloads.sourceforge.net/project/glew/glew/2.1.0/glew-2.1.0-win32.zip")
    set(GLEW_PATH "${LIBS_DIR}/glew-2.1.0")
    set(GLEW_ZIP_PATH "${LIBS_DIR}/glew-2.1.0.zip")

    if (NOT EXISTS "${GLEW_PATH}")
        file(MAKE_DIRECTORY "${LIBS_DIR}")

        message(STATUS "Downloading glew...")
        file(DOWNLOAD "${GLEW_URL}" "${GLEW_ZIP_PATH}")

        message(STATUS "Uncompressing glew...")
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E tar "xf" "${GLEW_ZIP_PATH}" --format=zip
            WORKING_DIRECTORY ${LIBS_DIR}
        )

        file(REMOVE "${GLEW_ZIP_PATH}")
    endif()

    set(GLEW_DIR ${GLEW_PATH} PARENT_SCOPE)
endfunction(DownloadGLEW)

macro(GetGLEW)
    ###############################################
    # Set env vars and download glew
    ###############################################

    if (NOT GLEW_PARENT_DIR)
        set(GLEW_PARENT_DIR "${CMAKE_CURRENT_LIST_DIR}")
    endif()

    DownloadGLEW(${GLEW_PARENT_DIR} GLEW_DIR)


    ###############################################
    # Find include dir
    ###############################################

    find_path(GLEW_INCLUDE_DIRS GL/glew.h HINTS ${GLEW_DIR}/include)
    if (GLEW_INCLUDE_DIRS)
        message(STATUS "Found GLEW include dir: ${GLEW_INCLUDE_DIRS}")
    else()
        message(FATAL_ERROR "Could not find GLEW include dir -- abort")
    endif()


    ###############################################
    # Find static library
    ###############################################

    if (GLEW_BUILD_x64)
        find_library(GLEW_LIBRARIES NAMES glew32s HINTS "${GLEW_DIR}/lib/Release/x64")
    else()
        find_library(GLEW_LIBRARIES NAMES glew32s HINTS "${GLEW_DIR}/lib/Release/Win32")
    endif()
    if (GLEW_LIBRARIES)
        message(STATUS "Found GLEW library: ${GLEW_LIBRARIES}")
    else()
        message(FATAL_ERROR "Could not find GLEW library -- abort")
    endif()


    ###############################################
    # Create library interface
    ###############################################

    if (NOT TARGET GLEW) 
        add_library(GLEW INTERFACE)
        target_include_directories(GLEW INTERFACE ${GLEW_INCLUDE_DIRS})
        target_link_libraries(GLEW INTERFACE ${GLEW_LIBRARIES})

        add_library(GLEW::GLEW ALIAS GLEW)
    endif()
endmacro(GetGLEW)
