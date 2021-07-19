#include "mini_map_widget.h"

void overlayImage(cv::Mat *src, cv::Mat *overlay, const cv::Point &location) {
    for (int y = max(location.y, 0); y < src->rows; ++y) {
        int fY = y - location.y;

        if (fY >= overlay->rows)
            break;

        for (int x = max(location.x, 0); x < src->cols; ++x) {
            int fX = x - location.x;

            if (fX >= overlay->cols)
                break;

            double opacity = ((double) overlay->data[fY * overlay->step + fX * overlay->channels() + 3]) / 255;

            for (int c = 0; opacity > 0 && c < src->channels(); ++c) {
                unsigned char overlayPx = overlay->data[fY * overlay->step + fX * overlay->channels() + c];
                unsigned char srcPx = src->data[y * src->step + x * src->channels() + c];
                src->data[y * src->step + src->channels() * x + c] = srcPx * (1. - opacity) + overlayPx * opacity;
            }
        }
    }
}

MiniMapWidget *wThis = nullptr;

MiniMapWidget::MiniMapWidget(QWidget *parent) : QMainWindow(parent) {
    genshin = new GenshinImpactMiniMap();
    setWindowTitle(tr("Genshin Impact MiniMap"));
    setObjectName("Mini Map Form");
    resize(ellipse_size + 20, ellipse_size + 20);
    move(55, 10);
    miniMap = new QLabel(this);
    miniMap->resize(size());
    setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
    setAttribute(Qt::WA_TranslucentBackground);
    loadLogos();
    setMiniMapLogo();

    keyHook = SetWindowsHookEx(WH_KEYBOARD_LL, switchMiniMapModes, nullptr, 0);
    connect(keyboardListener, &QTimer::timeout, this, &MiniMapWidget::keyListener);
    keyboardListener->start();

    wThis = this;
}

void MiniMapWidget::loadLogos() {
    logo = cv::imread("genshin_logo.png", cv::IMREAD_UNCHANGED);
    cv::cvtColor(logo, logo, cv::COLOR_BGRA2RGBA);
    transparentLogo = cv::imread("genshin_transparent.png", cv::IMREAD_UNCHANGED);
    cv::cvtColor(transparentLogo, transparentLogo, cv::COLOR_BGRA2RGBA);
    customLogo = cv::imread("custom_logo.png", cv::IMREAD_UNCHANGED);
    if (!customLogo.empty()) {
        cv::cvtColor(customLogo, customLogo, cv::COLOR_BGRA2RGBA);
    }
}

void MiniMapWidget::setMiniMapLogo() {
    if (!logo.empty()) {
        stopTimer();
        setMiniMapImage(logo);
    }
}

void MiniMapWidget::setMiniMapCustomLogo() {
    if (!customLogo.empty()) {
        stopTimer();
        setMiniMapImage(customLogo);
    }
}

void MiniMapWidget::setMiniMapImage(const cv::Mat &image) {
    auto pixMap = QPixmap::fromImage(
            QImage((unsigned char *) image.data, image.cols, image.rows, QImage::Format_RGBA8888));
    auto scaledPixMap = pixMap.scaled(miniMap->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    miniMap->setPixmap(scaledPixMap);
    miniMap->repaint();
}

void MiniMapWidget::inpaintMiniMap() {
    auto inpaintedMiniMap = genshin->getInpaintedMiniMap();
    cv::cvtColor(inpaintedMiniMap, inpaintedMiniMap, cv::COLOR_RGB2BGRA);
    cv::Mat maskedMiniMap;
    cv::copyTo(inpaintedMiniMap, maskedMiniMap, genshin->mask);
    setMiniMapImage(maskedMiniMap);
}

void MiniMapWidget::inpaintMiniMapTimer() {
    stopTimer();
    timer = new QTimer(this);
    timer->setInterval(genshin->updateMs);
    connect(timer, &QTimer::timeout, this, &MiniMapWidget::inpaintMiniMap);
    timer->start();
}

void MiniMapWidget::inpaintMiniMapWithLogo() {
    auto inpaintedMiniMap = genshin->getInpaintedMiniMap();
    cv::cvtColor(inpaintedMiniMap, inpaintedMiniMap, cv::COLOR_RGB2BGRA);
    cv::Mat maskedMiniMap;
    cv::copyTo(inpaintedMiniMap, maskedMiniMap, genshin->mask);
    overlayImage(&maskedMiniMap, &transparentLogo, cv::Point());
    setMiniMapImage(maskedMiniMap);
}

void MiniMapWidget::inpaintMiniMapWithLogoTimer() {
    stopTimer();
    timer = new QTimer(this);
    timer->setInterval(genshin->updateMs);
    connect(timer, &QTimer::timeout, this, &MiniMapWidget::inpaintMiniMapWithLogo);
    timer->start();
}

void MiniMapWidget::stopTimer() {
    if (timer) {
        timer->stop();
        delete timer;
        timer = nullptr;
    }
}
#include <iostream>
void MiniMapWidget::keyListener() {
    MSG msg;
    GetMessage(&msg, nullptr, 0, 0);
    TranslateMessage(&msg);
    DispatchMessageW(&msg);
}

LRESULT MiniMapWidget::switchMiniMapModes(int nCode, WPARAM wParam, LPARAM lParam) {
    auto key = (PKBDLLHOOKSTRUCT) lParam;
    if (wParam == WM_KEYDOWN && nCode == HC_ACTION) {
        switch (key->vkCode) {
            case 'X':
                wThis->miniMap->setHidden(!wThis->miniMap->isHidden());
                break;
            case VK_F7:
                if (wThis->customLogo.empty()){
                    wThis->setMiniMapLogo();
                }
                else{
                    if (wThis->customLogoFlag) {
                        wThis->setMiniMapLogo();
                        wThis->customLogoFlag = false;
                    } else {
                        wThis->setMiniMapCustomLogo();
                        wThis->customLogoFlag = true;
                    }
                }
                break;
            case VK_F8:
                if (wThis->inpaintLogoFlag) {
                    wThis->inpaintMiniMapTimer();
                    wThis->inpaintLogoFlag = false;
                } else {
                    wThis->inpaintMiniMapWithLogoTimer();
                    wThis->inpaintLogoFlag = true;
                }
                break;
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

void MiniMapWidget::closeEvent(QCloseEvent *event) {
    stopTimer();
    keyboardListener->stop();
    if (keyHook)
        UnhookWindowsHookEx(keyHook);
    event->accept();
}
