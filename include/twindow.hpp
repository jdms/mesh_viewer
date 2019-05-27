#ifndef TVIEWER_TWINDOW
#define TVIEWER_TWINDOW


#include <memory>
#include <string>
#include <vector>
#include <type_traits>

#include <tucanow/scene.hpp>
#include <tucanow/gui.hpp>

struct GLFWwindow;
struct WidgetData;

namespace tucanow {
    class Scene;
    class Gui;
}

// TODO: 
//
// Create method to determine whether gui should be used or not. 
//---------------------------------------------------------------
//
// E.g., enableGui()/disableGui().  Then when rendering or treating mouse
// interactions we should use isGuiEnabled() If the gui was not set,
// isGuiEnabled() should return false as well
//
//
// Think about the new fine grained control on glfw window
//--------------------------------------------------------
//
// Also, have a  look at glfw docs for guidance.
//
//
// Add method to call glfwSetWindowShouldClose(window, 1);    
//--------------------------------------------------------
//
// Actually, wrap other glfw functions or just add getter to window pointer? 
//---------------------------------------------------------------------------
//
// E.g.: 
//    glfwGetCursorPos(window, &xpos, &ypos);
//    glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1)
//
//
// Get rid of pdata_, move pscene and pgui to ` protected:` section
//-----------------------------------------------------------------
//
//
// Add method to set TWindow properties? 
//--------------------------------------------------------
//
// E.g.: 
//  GuiIsVisible, etc.

/// Create and manage an aplication window
class TWindow
{
    public:
        /// Get pointer to TWindow (singleton), returns nullptr in case of failure
        static TWindow* Get(int width, int height, std::string title = "tucanow::Scene Window");

        /// Destructor
        virtual ~TWindow();

        /// Deleted default copy ctor
        TWindow(TWindow &) = delete;

        /// Deleted default copy assigment
        TWindow& operator=(const TWindow &) = delete;

        /// Deleted default move ctor
        TWindow(TWindow &&) = delete;

        /// Deleted default move assigment
        TWindow& operator=(TWindow &&) = delete;

        /// Set window title
        bool setTitle( const std::string &title );

        /// Display default usage info
        void displayUsage();

        /// Get pointer to tucanow::Scene
        static tucanow::Scene* getScene();

        /// Get pointer to tucanow::Gui
        static tucanow::Gui* getGui();

        /// Get pointer to GLFWwindow
        static GLFWwindow* getGLFWwindow();

        /// Set new Gui (its type must derive from tucanow::Gui)
        static void setGui( std::shared_ptr<tucanow::Gui> gui );

        /// Render scene
        void renderScene();

        /// Render gui if non-null
        void renderGui();

        /// set glfw3 key callback
        static void setKeyCallback( void (*callback)(GLFWwindow* window, int key, int scancode, int action, int mods) );

        /// set glfw3 mouse button callback
        static void setMouseButtonCallback( void (*callback)(GLFWwindow* window, int button, int action, int mods) );

        /// set glfw3 cursor position callback
        static void setCursorPosCallback( void (*callback)(GLFWwindow* window, double xpos, double ypos) );

        /// set glfw3 mouse wheel callback
        static void setScrollCallback( void (*callback)(GLFWwindow* window, double xoffset, double yoffset) );

        /// set glfw3 framebuffer resize callback
        static void setFramebufferSizeCallback( void (*callback)(GLFWwindow *window, int width, int height) );

        /// set glfw3 window content scale callback
        static void setWindowContentScaleCallback( void (*callback)(GLFWwindow *window, float xscale, float yscale) );

        /// Make context current in glfw window
        void makeContextCurrent();

        /// Swap glfw buffers
        void swapBuffers();

        /// Pool glfw events
        void poolEvents();

        /// Inquire whether current window should be closed
        bool shouldClose();

        /// Run glfw3 window, track events, render OpenGL widget
        int run();

        /// TODO:
        /* bool setSceneShadersDir(std::string dir); */
        /* bool setGuiAssetsDir(std::string dir); */

    protected:
        // Methods:

        /// default glfw3 key callback
        static void defaultKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

        /// default glfw3 mouse button callback
        static void defaultMouseButtonCallback (GLFWwindow* window, int button, int action, int mods);

        /// default glfw3 cursor callback
        static void defaultCursorPosCallback(GLFWwindow* window, double xpos, double ypos);

        /// default glfw3 mouse wheel callback
        static void defaultScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

        /// default glfw3 framebuffer resize callback
        static void defaultFramebufferSizeCallback(GLFWwindow *window, int width, int height);

        /// default glfw3 window content scale callback
        static void defaultWindowContentScaleCallback(GLFWwindow *window, float xscale, float yscale);


        // Members:

        static std::shared_ptr<tucanow::Scene> pscene; ///< Holds a tucanow::Scene

        static std::shared_ptr<tucanow::Gui> pgui; ///< Holds a tucanow::Gui

        static GLFWwindow* main_window; ///< Points to memory managed by glfw3


    private: 
        /* static std::unique_ptr<WidgetData> pdata_; /// TWindow internal data */


        /* /// Initialize tucanow::Scene (call glew) */
        /* static void initializeScene( WidgetData* data ); */

        /// Initialize tucanow::Gui
        /* static void initializeGui( WidgetData* data ); */

        /// Private ctor for singleton pattern
        TWindow();
};


#endif
