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
    TCHAR path_file[256];
    for (size_t i = 0; i < num; i++) {
        _stprintf_s(path_file, path, i);
        IMAGE* picture = new IMAGE;
        loadimage(picture, path_file);
        frame_list.push_back(picture);
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
    timer += delta;
    if (timer >= internal) {
        idx_frame = (idx_frame + 1) % anim_atlas->frame_list.size();
        timer = 0;
    }
    putimage_alpha(x, y, anim_atlas->frame_list[idx_frame]);
}