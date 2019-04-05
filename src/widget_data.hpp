#ifndef TVIEWER_WIDGET_DATA
#define TVIEWER_WIDGET_DATA

#include <string>


struct WidgetData 
{
    int scene_width_ = 0;
    int scene_height_ = 0;

    std::string assets_dir_ = "./samples/assets/";
};


#endif
