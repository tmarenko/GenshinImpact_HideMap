#include "genshin_mini_map.h"
#include "windows.h"
#include <iostream>

#define PW_RENDERFULLCONTENT 0x00000002 // Properly capture DirectComposition window content


BOOL CALLBACK EnumWindowsFunc(HWND hwnd, LPARAM lParam) {
    auto *gwi = (GenshinWindowInfo *) lParam;
    TCHAR buf[1024]{};

    GetClassName(hwnd, buf, 100);
    if (!lstrcmp(buf, gwi->windowClass)) {
        GetWindowText(hwnd, buf, 100);
        if (!lstrcmp(buf, gwi->windowName)) {
            gwi->hwnd = hwnd;
            RECT windowRect;
            GetWindowRect(hwnd, &windowRect);
            gwi->width = windowRect.right - windowRect.left;
            gwi->height = windowRect.bottom - windowRect.top;
            std::cout << buf << " " << hwnd << " " << gwi->width << " " << gwi->height << std::endl;
        }
    }
    return TRUE;
}

GenshinImpactMiniMap::GenshinImpactMiniMap(std::string const &windowName, std::string const &windowClass,
                                           int updateMs) {
    this->updateMs = updateMs;
    GenshinWindowInfo gwi;
    gwi.windowName = windowName.c_str();
    gwi.windowClass = windowClass.c_str();
    EnumWindows(EnumWindowsFunc, reinterpret_cast<LPARAM>(&gwi));
    this->window = gwi.hwnd;
    this->width = gwi.width;
    this->height = gwi.height;
    createMiniMapMask();
}


void GenshinImpactMiniMap::getFrame(int screenWidth, int screenHeight) {
    frame.create(screenHeight, screenWidth, CV_8UC4);
    BITMAPINFOHEADER bi;
    bi.biSize = sizeof(BITMAPINFOHEADER);    //http://msdn.microsoft.com/en-us/library/windows/window/dd183402%28v=vs.85%29.aspx
    bi.biWidth = screenWidth;
    bi.biHeight = -screenHeight;  //this is the line that makes it draw upside down or not
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    auto hwndDC = GetWindowDC(window);
    auto saveDC = CreateCompatibleDC(hwndDC);

    auto bitMap = CreateCompatibleBitmap(hwndDC, screenWidth, screenHeight);
    SelectObject(saveDC, bitMap);

    PrintWindow(window, saveDC, PW_RENDERFULLCONTENT);
    GetDIBits(saveDC, bitMap, 0, screenHeight, frame.data, (BITMAPINFO *) &bi, DIB_RGB_COLORS);

    DeleteObject(bitMap);
    DeleteDC(saveDC);
    DeleteObject(hwndDC);
    ReleaseDC(window, hwndDC);
    cv::cvtColor(frame, frame, cv::COLOR_RGBA2RGB);
}

void GenshinImpactMiniMap::createMiniMapMask() {
    mask.create(height, width, CV_8UC1);
    mask = cv::Scalar(0, 0, 0);
    cv::ellipse(mask, cv::Point(165, 125),
                cv::Size(ellipse_size / 2 - 2, ellipse_size / 2 - 2), 0, 0,
                360, cv::Scalar(255, 255, 255),
                -1, cv::LINE_AA);
    mask = mask(miniMapRect);
}

cv::Mat GenshinImpactMiniMap::getInpaintedMiniMap() {
    getFrame(270, 270);
    cv::Mat miniMapImage = frame(miniMapRect);
    cv::inpaint(miniMapImage, mask, miniMapImage, 0, cv::INPAINT_TELEA);
    cv::medianBlur(miniMapImage, miniMapImage, 85);
    return miniMapImage;
}
