#include <string>
#include <vector>

#include <glog/logging.h>

#include "twindow.hpp"
#include "tucanow/scene.hpp"
#include "tucanow/sphere.hpp"


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


    // Set meshes to be visualized
    std::vector<float> vertices1, vertices2, normals1, normals2;
    std::vector<unsigned int> triangles1, triangles2;
    Sphere s1, s2; 

    s1.setCenter(.5f, .5f, .5f);
    s1.setRadius(1./4.f);
    s1.getMesh(vertices1, triangles1, normals1);

    s2.setCenter(-1./2., -1./2., -1./2.);
    s2.setRadius(1./4.);
    s2.getMesh(vertices2, triangles2, normals2);


    // Load meshes into scene
    auto scene = main_window->getScene();

    std::array<float, 3> bbox_origin = {-1.f, -1.f, -1.f};
    std::array<float, 3> bbox_size = {2.f, 2.f, 2.f};
    scene->setBoundingBox(bbox_origin, bbox_size);
    scene->focusCameraOnBoundingBox();

    scene->loadTriangleMesh(1, vertices1, triangles1, normals1);
    scene->setObjectColor(1, 1.0f, 0.f, 0.f);
    scene->setObjectShader(1, tucanow::ObjectShader::OnePassWireframe);

    /* scene->loadPointCloud(2, vertices2); */
    scene->loadTriangleMesh(2, vertices2, triangles2, normals2);
    scene->setObjectColor(2, 0.f, 0.f, 1.0f);


    // Run
    main_window->displayUsage();
    return main_window->run();
}
