#ifndef ANIMATION_H
#define ANIMATION_H

#include <graphics.h>
#include <vector>
void putimage_alpha(int x, int y, IMAGE* img);


class Atlas {
public:
    std::vector<IMAGE*> frame_list;
    Atlas(LPCTSTR path, int num);

    // ��ͼ���ع��캯��
    Atlas(LPCTSTR path) {
        IMAGE* picture = new IMAGE; // ����IMAGEָ��
        loadimage(picture, path); // ����ͼƬ
        frame_list.push_back(picture); // ָ����붯̬����
    }
    ~Atlas();
};

class Animation {
private:
    Atlas* anim_atlas;
    int internal;
    int idx_frame;
    int timer;

public:
    Animation(Atlas* atlas, int jiange);
    ~Animation();
    void showimage(int x, int y, int delta);
    Atlas* getanim_atlas(){
        return anim_atlas;
	}
};

// Helper function declaration

#endif // ANIMATION_H