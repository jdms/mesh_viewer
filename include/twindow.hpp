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
// Add methods to register new callbacks. 
//-----------------------------------------
//
// This is more useful for setting new keyboard mappings.  Also, is it useful
// to have a registerCallbacks() method or to just allow each individual
// callback mapper to update its callbak with glfw?
// 
//
// Think about the new fine grained control on glfw window
//--------------------------------------------------------
//
// Also, have a  look at glfw docs for guidance.
//
//
// Allow clients to properly set-up their own Guis (which must inherit from tucanow::Gui).
//----------------------------------------------------------------------------------------
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
        /// TWindow is a singleton, returns nullptr if fails
        static TWindow* Get(int width, int height, std::string title = "tucanow::Scene Window");

        /// Destructor
        ~TWindow();

        /// Deleted default copy ctor
        TWindow(TWindow &) = delete;

        /// Deleted default copy assigment
        TWindow& operator=(const TWindow &) = delete;

        /// Deleted default move ctor
        TWindow(TWindow &&) = delete;

        /// Deleted default move assigment
        TWindow& operator=(TWindow &&) = delete;

        /// Display default usage info
        void displayUsage();

        // TODO: this method seems redundant now
        /// Path to dir with widget's assets (textures, icons, etc.)
        bool setAssetsDir(std::string name);

        /// Get pointer to tucanow::Scene
        static tucanow::Scene* getScene();

        /// Get pointer to tucanow::Gui
        static tucanow::Gui* getGui();

        /// Set new Gui, type TGui must derive from tucanow::Gui
        template<typename TGui>
        static TGui* setGui( std::string tgui_assets_dir = {} );

        /// Render scene
        void renderScene();

        /// Render gui if non-null
        void renderGui();

        /// TODO: Implement --> Update glfw callbacks
        void registerCallbacks();

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

        /// glfw3 callback
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

        /// glfw3 callback
        static void mouseButtonCallback (GLFWwindow* window, int button, int action, int mods);

        /// glfw3 callback
        static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);

        /// glfw3 default mouse wheel callback
        static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

        /// glfw3 default framebuffer resize callback
        static void framebufferSizeCallback(GLFWwindow *window, int width, int height);


        // Members:

        static std::unique_ptr<tucanow::Scene> pscene; ///< Holds a tucanow::Scene, must be initialized after glew
        static std::unique_ptr<tucanow::Gui> pgui; ///< Holds a tucanow::Gui

    private: 
        static std::unique_ptr<WidgetData> pdata_; /// TWindow internal data

        static GLFWwindow* main_window; ///< Points to memory managed by glfw3


        /// Initialize tucanow::Scene (call glew)
        static void initializeScene( WidgetData* data );

        /// Initialize tucanow::Gui
        static void initializeGui( WidgetData* data );

        /// Private ctor for singleton pattern
        TWindow();
};


template<typename TGui>
TGui* TWindow::setGui(std::string tgui_assets_dir)
{
    if ( std::is_convertible<TGui*, tucanow::Gui*>::value )
    {
        if ( tgui_assets_dir.empty() )
        {
            pgui.reset( std::make_unique<TGui>() );
        }
        else
        {
            pgui.reset( std::make_unique<TGui>(tgui_assets_dir) );
        }
    }
    else
    {
        pgui.reset(nullptr);
    }

    return pgui.get();
}

#endif
