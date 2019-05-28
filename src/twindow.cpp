#include <iostream>

// Add missing OpenGL extensions in Windows
#if defined(_WIN32)
    #include <GL/glew.h>
#endif

#include "GLFW/glfw3.h"

#include "twindow.hpp"
#include "widget_data.hpp"
#include "tucanow/scene.hpp"
#include "tucanow/phong_gui.hpp"
#include "tucanow/misc.hpp"


GLFWwindow* TWindow::main_window = nullptr;
std::shared_ptr<tucanow::Scene> TWindow::pscene = nullptr;
std::shared_ptr<tucanow::Gui> TWindow::pgui = nullptr;
/* std::unique_ptr<WidgetData> TWindow::pdata_ = nullptr; */


TWindow::TWindow() = default;

TWindow::~TWindow()
{
    glfwDestroyWindow(main_window);
    glfwTerminate();
}

TWindow* TWindow::Get(int width, int height, std::string title)
{
    static bool already_initialized = false;

    if ( !already_initialized )
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
#ifdef __APPLE__
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#else
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#endif
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, 4);

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

        setKeyCallback(defaultKeyCallback); 
        setMouseButtonCallback(defaultMouseButtonCallback); 
        setCursorPosCallback(defaultCursorPosCallback); 
        setScrollCallback(defaultScrollCallback); 
        setFramebufferSizeCallback(defaultFramebufferSizeCallback);
        setWindowContentScaleCallback(defaultWindowContentScaleCallback);

        /////////////////////////////////////////////////////////////////
        // Initialize tucanow::Scene
        /////////////////////////////////////////////////////////////////

        // TODO: tidy up use of pointers
        /* pdata_.reset( new WidgetData() ); */

        /* pdata_->scene_width_ = fb_width; */
        /* pdata_->scene_height_= fb_height; */
        /* initializeScene(pdata_.get()); */

        /* Glew must be initialized before any tucanow object is created */
        tucanow::misc::initGlew();

        // macOS HighDpi mode creates a framebuffer that differs from the window size
        // https://www.glfw.org/docs/latest/window.html#window_fbsize
        int fb_width = 0;
        int fb_height = 0;
        glfwGetFramebufferSize( main_window, &fb_width, &fb_height );

        // keep track of the window/framebuffer ratio (for when using HighDPI monitors)
        // https://www.glfw.org/docs/latest/window.html#window_scale
        float xscale = 1.0f, yscale = 1.0f;

        #if defined(TWINDOW_GLFW3_VERSION_3_3_OR_GREATER)
            glfwGetWindowContentScale(main_window, &xscale, &yscale);
            std::cout << "--> Managed (glfw) content scale: xscale = " << xscale << ", yscale = " << yscale << std::endl;
        #else
            if ( (fb_width > 0) && (fb_height > 0) )
            {
                xscale = static_cast<float>(fb_width)/static_cast<float>(width);
                yscale = static_cast<float>(fb_height)/static_cast<float>(height);
            }
            std::cout << "--> Computed (fixed) content scale: xscale = " << xscale << ", yscale = " << yscale << std::endl;
        #endif

        pscene = std::make_shared<tucanow::Scene>();
        if ( pscene == nullptr )
        {
            std::cerr << "Failed to create the tucanow::Scene" << std::endl;
            glfwTerminate();
            return nullptr;
        }
        // TODO: change tucanow::Scene::initialize() into a factory method
        pscene->initialize(fb_width, fb_height);
        pscene->setScreenScale( xscale, yscale );

        pgui = tucanow::PhongGui::Get( *pscene );

        /* if ( pscene != nullptr ) */
        /* if ( (fb_width > 0) && (fb_height > 0) ) */
        /* { */
        /* pscene->setScreenScale( */ 
        /* static_cast<float>(fb_width)/static_cast<float>(width), */ 
        /* static_cast<float>(fb_height)/static_cast<float>(height) */ 
        /* ); */
        /* } */

        /* pscene->getScreenScale( xscale, yscale ); */
        /* std::cout << "scene xscale: " << xscale << ", scene yscale: " << yscale << std::endl << std::flush; */ 
        /* glfwGetWindowContentScale(main_window, &xscale, &yscale); */
        /* std::cout << "glfw xscale: " << xscale << ", glfw yscale: " << yscale << std::endl << std::flush; */ 

        already_initialized = true;
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

/* // TODO: this method seems redundant now */
/* /// Path to dir with widget's assets (textures, icons, etc.) */
/* bool setAssetsDir(std::string name); */

/* bool TWindow::setAssetsDir(std::string dirname) */
/* { */
/*     if ( dirname.empty() ) */
/*     { */
/*         return false; */
/*     } */

/*     pdata_->assets_dir_ = dirname; */

/*     return true; */
/* } */

tucanow::Scene* TWindow::getScene()
{
    return pscene.get();
}

tucanow::Gui* TWindow::getGui()
{
    return pgui.get();
}

GLFWwindow* TWindow::getGLFWwindow()
{
    return main_window;
}

void TWindow::setGui( std::shared_ptr<tucanow::Gui> gui )
{
    pgui = gui;
}

/* void TWindow::initializeScene(WidgetData *data) */
/* { */
/*     /1* Glew must be initialized before any tucanow object is created *1/ */
/*     tucanow::misc::initGlew(); */

/*     pscene = std::make_shared<tucanow::Scene>(); */
/*     pgui = std::make_shared<tucanow::PhongGui>( *pscene ); */

/*     pscene->initialize(data->scene_width_, data->scene_height_); */
/* } */

/* void TWindow::initializeGui(WidgetData *data) */
/* { */
/*     if ( pgui ) */
/*         pgui->initialize(data->scene_width_, data->scene_width_, data->assets_dir_); */
/* } */

void TWindow::defaultKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);    

    if ( !pscene )
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

void TWindow::defaultMouseButtonCallback (GLFWwindow* window, int button, int action, int mods)
{
    if( !pscene )
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
    double xposd, yposd;
    glfwGetCursorPos(window, &xposd, &yposd);
    float xpos = static_cast<float>(xposd);
    float ypos = static_cast<float>(yposd);

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        if ( pgui )
            if ( pgui->leftButtonPressed(xpos, ypos) )
                return;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        if ( pgui )
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

void TWindow::defaultCursorPosCallback(GLFWwindow* window, double xposd, double yposd)
{
    if( !pscene )
    {
        return;
    }

    float xpos = static_cast<float>(xposd);
    float ypos = static_cast<float>(yposd);

/* <<<<<<< HEAD */
    /* if ( pscene->getGUI()->cursorMove (xpos, ypos) ) */
    /* { */
    /*     return; */
    /* } */
/* ======= */
    if ( pgui )
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

void TWindow::defaultScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    if( !pscene )
    {
        return;
    }

    if (yoffset > 0)
    {
        pscene->increaseCameraZoom();
    }
    else if (yoffset < 0)
    {
        pscene->decreaseCameraZoom();
    }
}

void TWindow::defaultFramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    if ( pscene )
    {
        pscene->setViewport(width, height);
    }

    if ( pgui )
    {
        pgui->setViewport(width, height);
    }
}

void TWindow::defaultWindowContentScaleCallback(GLFWwindow *window, float xscale, float yscale)
{
    std::cout << "--> Managed (glfw) content scale: xscale = " << xscale << ", yscale = " << yscale << std::endl;
    if ( pscene )
        pscene->setScreenScale(xscale, yscale);
}

bool TWindow::setTitle( const std::string &title )
{
    if ( title.empty() )
    {
        return false;
    }

    glfwSetWindowTitle(main_window, title.c_str());

    return true;
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
    std::cout << std::endl;
}

void TWindow::renderScene()
{
    if ( pscene )
    {
        pscene->render();
    }
}

void TWindow::renderGui()
{
    if ( pgui )
    {
        pgui->render();
    }
}

/* void TWindow::registerCallbacks() */
/* { */
/*     glfwSetKeyCallback(main_window, keyCallback); */ 
/*     glfwSetMouseButtonCallback(main_window, mouseButtonCallback); */ 
/*     glfwSetCursorPosCallback(main_window, cursorPosCallback); */ 
/*     glfwSetScrollCallback(main_window, scrollCallback); */ 
/*     glfwSetFramebufferSizeCallback(main_window, framebufferSizeCallback); */
/* } */

/// set glfw3 key callback
void TWindow::setKeyCallback( void (*callback)(GLFWwindow* window, int key, int scancode, int action, int mods) )
{
    glfwSetKeyCallback(main_window, callback); 
}

/// set glfw3 mouse button callback
void TWindow::setMouseButtonCallback( void (*callback)(GLFWwindow* window, int button, int action, int mods) )
{
    glfwSetMouseButtonCallback(main_window, callback); 
}

/// set glfw3 cursor position callback
void TWindow::setCursorPosCallback( void (*callback)(GLFWwindow* window, double xpos, double ypos) )
{
    glfwSetCursorPosCallback(main_window, callback); 
}

/// set glfw3 mouse wheel callback
void TWindow::setScrollCallback( void (*callback)(GLFWwindow* window, double xoffset, double yoffset) )
{
    glfwSetScrollCallback(main_window, callback); 
}

/// set glfw3 framebuffer resize callback
void TWindow::setFramebufferSizeCallback( void (*callback)(GLFWwindow *window, int width, int height) )
{
    glfwSetFramebufferSizeCallback(main_window, callback);
}

/// set glfw3 window content scale callback
void TWindow::setWindowContentScaleCallback( void (*callback)(GLFWwindow *window, float xscale, float yscale) )
{
    #if defined(TWINDOW_GLFW3_VERSION_3_3_OR_GREATER)
        std::cout << "--> Using glfw3 native window content scaling" << std::endl;
        glfwSetWindowContentScaleCallback(main_window, callback);
    #endif
}

void TWindow::makeContextCurrent()
{
    glfwMakeContextCurrent(main_window);
}

void TWindow::swapBuffers()
{
    glfwSwapBuffers( main_window );
}

void TWindow::poolEvents()
{
    glfwPollEvents();
}

bool TWindow::shouldClose()
{
    return glfwWindowShouldClose(main_window);
}

int TWindow::run()
{
    /* initializeGui(pdata_.get()); */

    glEnable(GL_MULTISAMPLE);

    /* registerCallbacks(); */

    glfwSetInputMode(main_window, GLFW_STICKY_KEYS, true);

    renderScene();
    renderGui();
    /* glfwSwapBuffers( main_window ); */
    swapBuffers();

    /* while (!glfwWindowShouldClose(main_window)) */
    while( !shouldClose() )
    {
        /* glfwMakeContextCurrent(main_window); */
        makeContextCurrent();
        renderScene();
        renderGui();
        /* glfwSwapBuffers( main_window ); */
        swapBuffers();

        /* glfwPollEvents(); */
        poolEvents();
    }

    glDisable(GL_MULTISAMPLE);

    return 0;
}
