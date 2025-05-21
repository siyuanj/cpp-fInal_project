#ifndef ANIMATION_H
#define ANIMATION_H

#include <graphics.h>
#include <vector>

class Atlas {
public:
    std::vector<IMAGE*> frame_list;
    Atlas(LPCTSTR path, int num);
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
};

// Helper function declaration
void putimage_alpha(int x, int y, IMAGE* img);

#endif // ANIMATION_H