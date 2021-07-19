#ifndef GENSHINIMPACTMINIMAP_GENSHIN_MINI_MAP_H
#define GENSHINIMPACTMINIMAP_GENSHIN_MINI_MAP_H

#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/photo.hpp"
#include "windows.h"

const int ellipse_size = 210;


class GenshinImpactMiniMap {
public:
    explicit GenshinImpactMiniMap(std::string const &windowName = "Genshin Impact",
                                  std::string const &windowClass = "UnityWndClass",
                                  int updateMs = 30);

    int updateMs;

    cv::Mat getInpaintedMiniMap();

    cv::Mat mask;


private:
    cv::Rect miniMapRect = cv::Rect(60, 20, ellipse_size, ellipse_size);
    HWND window;
    int width;
    int height;
    cv::Mat frame;

    void createMiniMapMask();

    void getFrame(int screenWidth, int screenHeight);
};

typedef struct {
    const char *windowName;
    const char *windowClass;
    HWND hwnd;
    int width;
    int height;
} GenshinWindowInfo;

#endif //GENSHINIMPACTMINIMAP_GENSHIN_MINI_MAP_H