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

        /// Display usage info
        void displayUsage();

        /// Path to dir with widget's assets (textures, icons, etc.)
        bool setAssetsDir(std::string name);

        /// Get pointer to tucanow::Scene
        tucanow::Scene* getScene();

        /// Get pointer to tucanow::Gui
        tucanow::Gui* getGui();

        /// Set new gui
        template<typename TGui>
        TGui* setGui( std::string tgui_assets_dir = {} );

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


    private: 
        static std::unique_ptr<WidgetData> pdata_; /// TWindow internal data

        static GLFWwindow* main_window; ///< Points to memory managed by glfw3
        static std::unique_ptr<tucanow::Scene> pscene; ///< Must be initialized after glew
        static std::unique_ptr<tucanow::Gui> pgui;


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
