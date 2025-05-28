#include "Animation.h"
#include <windows.h>
#include <stdexcept> // Ϊ��ʹ�� std::runtime_error

void putimage_alpha(int x, int y, IMAGE* img) {
    if (!img) {
        // ����imgΪ�յ�����������ӡ������Ϣ��ֱ�ӷ���
        fprintf(stderr, "Error: putimage_alpha called with null IMAGE pointer.\n");
        return;
    }
    int w = img->getwidth();
    int h = img->getheight();
    AlphaBlend(GetImageHDC(NULL), x, y, w, h,
        GetImageHDC(img), 0, 0, w, h,
        { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA });
}

Atlas::Atlas(LPCTSTR path, int num) {
    TCHAR path_file[256];
    for (size_t i = 0; i < num; i++) {
        int result = _stprintf_s(path_file, sizeof(path_file) / sizeof(TCHAR), path, i);
        if (result < 0) {
            // _stprintf_s ʧ�ܣ��������
            fprintf(stderr, "Error: _stprintf_s failed to format path.\n");
            // ����ѡ���׳��쳣����������ʽ�������
            throw std::runtime_error("Failed to format image path.");
        }

        IMAGE* picture = new IMAGE;
        if (picture == nullptr) {
            // new IMAGE ʧ�ܣ��ڴ����ʧ��
            fprintf(stderr, "Error: Failed to allocate memory for IMAGE object for path: %s\n", path_file);
            // �׳��쳣���ȡ�����������ʩ
            throw std::bad_alloc();
        }

        // ���� loadimage ����һ��״̬������룬������ʧ��ʱ����ȫ�ִ���״̬
        // EasyX �� loadimage ͨ����ʧ��ʱ��ʹ IMAGE ��������Ч״̬��
        // ���ҿ���ͨ�� GetLastError() ���ض��� EasyX �������ṩ������Ϣ��
        // ��������ֱ�ӵ��ã���������±�����˵����������� loadimage �ڲ����������
        loadimage(picture, path_file);

        // ��� IMAGE �����Ƿ���Ч����� IMAGE ���ṩ�������ķ�����
        // ���磬���Լ���Ⱥ͸߶��Ƿ����0
        if (picture->getwidth() == 0 || picture->getheight() == 0) {
            fprintf(stderr, "Error: loadimage failed for path: %s. Image might be missing or corrupted.\n", path_file);
            // �����ѷ���� picture ����
            delete picture;
            // ����ѡ���׳��쳣���¼����������֡
            // throw std::runtime_error("Failed to load image.");
            continue; // ��������𻵵Ļ�ȱʧ��ͼ��
        }

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
    if (anim_atlas == nullptr) {
        fprintf(stderr, "Error: Animation created with null Atlas pointer.\n");
        throw std::invalid_argument("Atlas pointer cannot be null for Animation.");
    }
    if (anim_atlas->frame_list.empty()) {
        fprintf(stderr, "Warning: Animation created with an Atlas that has no frames.\n");
        // ��������£�showimage �е�ȡģ�������ܻ������⣬��� frame_list.size() Ϊ0
        // idx_frame ��ʼ��Ϊ0�������� frame_list[0] ��Խ�硣
        // ����� showimage ��Ҳ�������������
    }
}

Animation::~Animation() {}

void Animation::showimage(int x, int y, int delta) {
    if (anim_atlas == nullptr || anim_atlas->frame_list.empty()) {
        // ���ͼ��Ϊ�ջ�û��֡����ִ���κβ������ӡ����
        // fprintf(stderr, "Error: Cannot show image, Atlas is null or empty.\n");
        return;
    }
    timer += delta;
    if (timer >= internal) {
        //idx_frame = (idx_frame + 1) % anim_atlas->frame_list.size(); ���������
        idx_frame = (static_cast<size_t>(idx_frame) + 1) % anim_atlas->frame_list.size();
        // ʹ�� static_cast<size_t> �� idx_frame ת��Ϊ�� anim_atlas->frame_list.size() ��ͬ�����ͣ�size_t��
        // ��ȷ�������ڸ�������ͷ�Χ�ڽ��У����������
        timer = 0;
    }
    
    // �ڷ���frame_list֮ǰ���ٴ�ȷ��idx_frame����Ч��
    if (idx_frame < anim_atlas->frame_list.size()) {
        putimage_alpha(x, y, anim_atlas->frame_list[idx_frame]);
    } else {
        // ������������ϲ�Ӧ�÷��������ȡģ������ȷ�Ļ�
        fprintf(stderr, "Error: idx_frame out of bounds in showimage.\n");
    }
}