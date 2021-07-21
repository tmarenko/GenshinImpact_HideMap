#ifndef GENSHINIMPACTMINIMAP_MINI_MAP_WIDGET_H
#define GENSHINIMPACTMINIMAP_MINI_MAP_WIDGET_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QCloseEvent>
#include "genshin_mini_map.h"

class MiniMapWidget : public QMainWindow {
Q_OBJECT

public:
    explicit MiniMapWidget(QWidget *parent = nullptr);

    void inpaintMiniMapTimer();

    void inpaintMiniMapWithLogoTimer();

    void setMiniMapLogo();

    void setMiniMapCustomLogo();

private:
    GenshinImpactMiniMap *genshin;
    cv::Mat logo;
    cv::Mat transparentLogo;
    cv::Mat customLogo;
    QLabel *miniMap = nullptr;
    QTimer *timer = nullptr;
    QTimer *mapTimer = new QTimer(this);
    QTimer *keyboardListener = new QTimer(this);
    HHOOK keyHook{};
    bool customLogoFlag = false;
    bool inpaintLogoFlag = false;
    bool isHidden = false;

    void loadLogos();

    void setMiniMapImage(const cv::Mat &image);

    void stopTimer();

    static LRESULT __stdcall switchMiniMapModes(int nCode, WPARAM wParam, LPARAM lParam);

    void closeEvent(QCloseEvent *event);

private slots:

    void inpaintMiniMap();

    void inpaintMiniMapWithLogo();

    static void keyListener();

    static void mapListener();
};

#endif //GENSHINIMPACTMINIMAP_MINI_MAP_WIDGET_H