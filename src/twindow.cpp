#include <iostream>

// Add missing OpenGL extensions in Windows
#if defined(_WIN32)
    #include <GL/glew.h>
#endif

#include "GLFW/glfw3.h"

#include "twindow.hpp"
#include "widget_data.hpp"
#include "tucanow/scene.hpp"
#include "tucanow/gui.hpp"
#include "tucanow/misc.hpp"


GLFWwindow* TWindow::main_window = nullptr;
std::unique_ptr<tucanow::Scene> TWindow::pscene = nullptr;
std::unique_ptr<tucanow::Gui> TWindow::pgui = nullptr;
std::unique_ptr<WidgetData> TWindow::pdata_ = nullptr;


TWindow::TWindow() = default;

TWindow::~TWindow()
{
    glfwDestroyWindow(main_window);
    glfwTerminate();
}

TWindow* TWindow::Get(int width, int height, std::string title)
{
    if ( (width < 1) || (height < 1) )
    {
        return nullptr;
    }


    /////////////////////////////////////////////////////////////////
    // Initialize glfw
    /////////////////////////////////////////////////////////////////

    if (!glfwInit()) 
    {
        std::cerr << "Failed to init glfw" << std::endl;
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 8);

    main_window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (main_window == nullptr)
    {
        std::cerr << "Failed to create the GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(main_window);

    std::cout << std::endl;
    std::cout << "*** Diagnostics:" << std::endl;

    const GLubyte* openGLRenderer = glGetString(GL_RENDERER);
    const GLubyte* openGLVersion = glGetString(GL_VERSION);
    std::cout << "--> Renderer: " << openGLRenderer << std::endl;
    std::cout << "--> OpenGL version in use: " << openGLVersion << std::endl;

    GLint bufs, samples;
    glGetIntegerv(GL_SAMPLE_BUFFERS, &bufs);
    glGetIntegerv(GL_SAMPLES, &samples);
    std::cout << "--> MSAA: buffers = " << bufs << ", samples = " << samples << std::endl;

    // macOS highdpi mode creates a framebuffer that differs from the window size
    int fb_width = 0;
    int fb_height = 0;
    glfwGetFramebufferSize( main_window, &fb_width, &fb_height );


    /////////////////////////////////////////////////////////////////
    // Initialize tucanow::Scene
    /////////////////////////////////////////////////////////////////

    // TODO: tidy up use of pointers
    pdata_.reset( new WidgetData() );

    pdata_->scene_width_ = fb_width;
    pdata_->scene_height_= fb_height;
    initializeScene(pdata_.get());

    if ( pscene != nullptr )
        if ( (fb_width > 0) && (fb_height > 0) )
        {
            pscene->setScreenScale( 
                    static_cast<float>(fb_width)/static_cast<float>(width), 
                    static_cast<float>(fb_height)/static_cast<float>(height) 
                    );
        }


    /////////////////////////////////////////////////////////////////
    // Create singleton (based on Scott Meyers' singleton pattern)
    /////////////////////////////////////////////////////////////////
    static TWindow single_instance;

    return &single_instance;
}

/* bool TWindow::setMesh( */
/*         int id, */
/*         const std::vector<float> &vertices, */
/*         const std::vector<unsigned int> &triangles, */
/*         const std::vector<float> &normals, */
/*         const std::vector<float> &colors_rgb */
/*         ) */
/* { */
    /* /1* if ( vertices.empty() || (vertices.size() % 3 != 0) ) *1/ */
    /* /1* { *1/ */
    /* /1*     return false; *1/ */
    /* /1* } *1/ */

    /* /1* if ( !triangles.empty() ) *1/ */
    /* /1* { *1/ */
    /* /1*     if ( triangles.size() % 3 != 0 ) *1/ */
    /* /1*     { *1/ */
    /* /1*         return false; *1/ */
    /* /1*     } *1/ */
    /* /1* } *1/ */

    /* /1* if ( !normals.empty() ) *1/ */
    /* /1* { *1/ */
    /* /1*     if ( normals.size() != vertices.size() ) *1/ */
    /* /1*     { *1/ */
    /* /1*         return false; *1/ */
    /* /1*     } *1/ */
    /* /1* } *1/ */

    /* /1* if ( !colors_rgb.empty() ) *1/ */
    /* /1* { *1/ */
    /* /1*     if ( colors_rgb.size() % 3 != 0 ) *1/ */
    /* /1*     { *1/ */
    /* /1*         return false; *1/ */
    /* /1*     } *1/ */

    /* /1*     if ( colors_rgb.size() != vertices.size() ) *1/ */
    /* /1*     { *1/ */
    /* /1*         return false; *1/ */
    /* /1*     } *1/ */
    /* /1* } *1/ */

    /* /1* pdata_->indices.insert(id); *1/ */
    /* /1* pdata_->vertices[id] = vertices; *1/ */
    /* /1* pdata_->triangles[id] = triangles; *1/ */
    /* /1* pdata_->normals[id] = normals; *1/ */
    /* /1* pdata_->colors_rgb[id] = colors_rgb; *1/ */
    /* pdata_->mesh_is_initialized_ = true; */

    /* bool success = pscene->loadMesh(id, vertices, triangles, normals); */
    /* success &= pscene->setObjectColorsRGB(id, colors_rgb); */
    /* /1* return true; *1/ */
/* } */

/* bool TWindow::openPLY(int object_id, std::string filename) */
/* { */
/*     /1* if ( filename.empty() ) *1/ */
/*     /1* { *1/ */
/*     /1*     return false; *1/ */
/*     /1* } *1/ */

/*     /1* pdata_->model_filename_ = filename; *1/ */
/*     pdata_->mesh_is_initialized_ = true; */

/*     /1* return true; *1/ */
/*     return pscene->loadPLY(object_id, filename); */
/* } */

bool TWindow::setAssetsDir(std::string dirname)
{
    if ( dirname.empty() )
    {
        return false;
    }

    pdata_->assets_dir_ = dirname;

    return true;
}

tucanow::Scene* TWindow::getScene()
{
    return pscene.get();
}

tucanow::Gui* TWindow::getGui()
{
    return pgui.get();
}

void TWindow::initializeScene(WidgetData *data)
{
    /* Glew must be initialized before any tucanow object is created */
    tucanow::misc::initGlew();

    pscene.reset( new tucanow::Scene() );
    pgui.reset( new tucanow::Gui( *pscene ) );

    pscene->initialize(data->scene_width_, data->scene_height_);
}

void TWindow::initializeGui(WidgetData *data)
{
    pgui->initialize(data->scene_width_, data->scene_width_, data->assets_dir_);
}

void TWindow::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);    

    if ( pscene == nullptr )
        return;

    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        pscene->resetCamera();
    }

    if (key == GLFW_KEY_H && action == GLFW_PRESS)
    {
        pscene->toggleHeadlight();
    }

    if (key == GLFW_KEY_W && action == GLFW_PRESS)
    {
        pscene->toggleRenderWireframe();
    }
}

void TWindow::mouseButtonCallback (GLFWwindow* window, int button, int action, int mods)
{
    if( pscene == nullptr )
        return;

/* <<<<<<< HEAD */
    /* if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) */
    /* { */
    /*     if (pscene->getGUI()->leftButtonPressed (xpos, ypos)) */
    /*         return; */
    /* } */

    /* if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) */
    /* { */
        /* pscene->getGUI()->leftButtonReleased (xpos, ypos); */
    /* } */
/* ======= */
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        if ( pgui->leftButtonPressed(xpos, ypos) )
            return;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        pgui->leftButtonReleased(xpos, ypos);
    }
/* >>>>>>> master */

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        pscene->rotateCamera(xpos, ypos);
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        pscene->stopRotateCamera();
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        pscene->translateCamera(xpos, ypos);
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        pscene->stopTranslateCamera();
    }

    if (button == GLFW_MOUSE_BUTTON_MIDDLE)
    {
        if (action == GLFW_PRESS)
        {
            pscene->rotateLight(xpos, ypos);
        }
        else if (action == GLFW_RELEASE)
        {
            pscene->stopRotateLight();
        }
    }
}

void TWindow::cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    if( pscene == nullptr )
        return;

/* <<<<<<< HEAD */
    /* if ( pscene->getGUI()->cursorMove (xpos, ypos) ) */
    /* { */
    /*     return; */
    /* } */
/* ======= */
    if ( pgui->cursorMove(xpos, ypos) )
    {
        return;
    }
/* >>>>>>> master */

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
    {
        pscene->rotateCamera(xpos, ypos);
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
    {
        pscene->translateCamera(xpos, ypos);
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_3) == GLFW_PRESS)
    {
        pscene->rotateLight(xpos, ypos);
    }

}

void TWindow::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    if( pscene == nullptr )
        return;

    if (yoffset > 0)
    {
        pscene->increaseCameraZoom();
    }
    else if (yoffset < 0)
    {
        pscene->decreaseCameraZoom();
    }
}

void TWindow::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    if( pscene == nullptr )
        return;

    pscene->setViewport(width, height);
    pgui->setViewport(width, height);
}

void TWindow::displayUsage()
{
    std::cout << std::endl << std::endl;
    std::cout << " *********************************************** " << std::endl;
    std::cout << " *** Usage:"                                       << std::endl;
    std::cout << " *********************************************** " << std::endl;
    std::cout << std::endl;
    std::cout << " Left mouse button   : rotate trackball"           << std::endl;
    std::cout << " Right mouse button  : translate trackball"        << std::endl;
    std::cout << " Middle mouse button : change light direction"     << std::endl;
    std::cout << std::endl;
    /* std::cout << " W : render wireframe                            " << std::endl; */
    std::cout << " H : alternate between headlight and fixed light " << std::endl;
    std::cout << " R : reset trackball and light direction"          << std::endl;
    std::cout << " *********************************************** " << std::endl;
}

int TWindow::run()
{
    initializeGui(pdata_.get());

    glEnable(GL_MULTISAMPLE);

    glfwSetKeyCallback(main_window, keyCallback); 
    glfwSetMouseButtonCallback(main_window, mouseButtonCallback); 
    glfwSetCursorPosCallback(main_window, cursorPosCallback); 
    glfwSetScrollCallback(main_window, scrollCallback); 
    glfwSetFramebufferSizeCallback(main_window, framebufferSizeCallback);
    glfwSetInputMode(main_window, GLFW_STICKY_KEYS, true);

    if( pscene != nullptr )
    {
        pscene->render();
        pgui->render();
    }
    glfwSwapBuffers( main_window );

    while (!glfwWindowShouldClose(main_window))
    {
        glfwMakeContextCurrent(main_window);
        if( pscene != nullptr )
        {
            pscene->render();
            pgui->render();
        }
        glfwSwapBuffers( main_window );

        glfwPollEvents();
    }

    glDisable(GL_MULTISAMPLE);

    return 0;
}
