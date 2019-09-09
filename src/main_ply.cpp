#include <string>

#include <glog/logging.h>

#include "twindow.hpp"
#include "tucanow/scene.hpp"

int main(int, char** argv)
{
    // Initialize google::glog
    FLAGS_logtostderr = true;
    FLAGS_colorlogtostderr = true;
    FLAGS_log_dir = ".";
    google::InitGoogleLogging(argv[0]);
 
    // Create main window
    const int window_width = 800;
    const int window_height = 800;
    std::string window_title = "A Simple Mesh Viewer";

    auto main_window = TWindow::Get(window_width, window_height);
    main_window->setTitle(window_title);
    /* main_window->setAssetsDir("./samples/assets/"); */


    // Load ply into scene
    auto scene = main_window->getScene();

    int object_id = 1;
    scene->loadPLY(object_id, "./samples/models/toy.ply");
    scene->focusCameraOnObject(object_id);


    // Run
    main_window->displayUsage();
    return main_window->run();
}
