#include <iostream>

#include <GL/glew.h>

#include "tucanow_misc.hpp"


namespace tucanow {
namespace misc {


void initGlew()
{
    static bool first_run = true;
    if (first_run)
    {
        glewExperimental = true;
        GLenum glewInitResult = glewInit();
        if (GLEW_OK != glewInitResult)
        {
            std::cerr << "\nError: " << glewGetErrorString(glewInitResult) << std::endl << std::flush;
            exit(EXIT_FAILURE);
        }

        first_run = false;
    }
}

}
}
