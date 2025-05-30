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
    TCHAR path_file[256]; // // ���ڴ洢����ͼ���ļ�������·��
    // ѭ������ÿһ֡ͼ��

    for (size_t i = 0; i < num; i++) {
        _stprintf_s(path_file, path, i); // ��ʽ��ͼ���ļ�·��
        IMAGE* picture = new IMAGE; // �����µ� IMAGE ����
        loadimage(picture, path_file); // ����ͼ���ļ��� IMAGE ����
        frame_list.push_back(picture); // �����ص�ͼ��֡��ӵ��б���
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
    timer += delta; // �����ڲ���ʱ��
    // �����ʱ���������趨��֡���
    if (timer >= internal) {
        // ���µ���һ֡������������һ֡��ѭ���ص�һ֡
        idx_frame = (idx_frame + 1) % anim_atlas->frame_list.size();
        timer = 0; // ���ü�ʱ��
    }
    // ʹ�� putimage_alpha ������ָ��λ�û��Ƶ�ǰ֡ͼ�� (֧�� Alpha ͸��)
    putimage_alpha(x, y, anim_atlas->frame_list[idx_frame]);
}