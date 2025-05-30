#include "Animation.h"
#include <windows.h>

void putimage_alpha(int x, int y, IMAGE* img) {
    int w = img->getwidth();
    int h = img->getheight();
    AlphaBlend(GetImageHDC(NULL), x, y, w, h,
        GetImageHDC(img), 0, 0, w, h,
        { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA });
}

Atlas::Atlas(LPCTSTR path, int num) {
    TCHAR path_file[256]; // // 用于存储单个图像文件的完整路径
    // 循环加载每一帧图像

    for (size_t i = 0; i < num; i++) {
        _stprintf_s(path_file, path, i); // 格式化图像文件路径
        IMAGE* picture = new IMAGE; // 创建新的 IMAGE 对象
        loadimage(picture, path_file); // 加载图像文件到 IMAGE 对象
        frame_list.push_back(picture); // 将加载的图像帧添加到列表中
    }
}

Atlas::~Atlas() {
    for (size_t i = 0; i < frame_list.size(); i++) {
        delete frame_list[i];
    }
}

Animation::Animation(Atlas* atlas, int jiange)
    : anim_atlas(atlas), internal(jiange), idx_frame(0), timer(0) {
}

Animation::~Animation() {}

void Animation::showimage(int x, int y, int delta) {
    timer += delta; // 更新内部计时器
    // 如果计时器超过了设定的帧间隔
    if (timer >= internal) {
        // 更新到下一帧，如果到达最后一帧则循环回第一帧
        idx_frame = (idx_frame + 1) % anim_atlas->frame_list.size();
        timer = 0; // 重置计时器
    }
    // 使用 putimage_alpha 函数在指定位置绘制当前帧图像 (支持 Alpha 透明)
    putimage_alpha(x, y, anim_atlas->frame_list[idx_frame]);
}