# Genshin Impact Hide Map
Hides in-game map for better immersion

## Preview (static and fancy)
![preview.png](preview.png)
![preview.gif](preview.gif)

## Installation and usage

Download from
[latest release](https://github.com/tmarenko/GenshinImpact_HideMap/releases)
and run `GenshinImpactMiniMap.exe`

**Currently supports only 1920x1080 screen resolution**

In order to see overlay you should run Genshin Impact in windowed mode:
- Press `Alt+Enter` or add `--popupwindow` argument to game's link
- Resize window as borderless 
  (f.e. [Borderless Gaming](https://github.com/Codeusa/Borderless-Gaming/releases))
  

**Controls:**

- Press `X` key to hide/show overlay
- Press `F7` key to show static overlay
- Press `F8` key to show dynamic overlay

You can also add yours overlay as `custom_logo.png`

## Building

Via `cmake`, requires `opencv` and `qt5` libraries