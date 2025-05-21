#pragma once
#ifndef SUN_H
#define SUN_H

#include "Animation.h"

extern int sun_count;

class Sun {
private:
    POINT position;
    POINT target;
    Animation* anim;
    bool is_active;
    const double MOVE_SPEED = 5.0;

public:
    Sun(POINT pos, Atlas* atlas);
    ~Sun();
    void Update(int delta);
    void Draw();
    bool IsActive() const;
};

#endif // SUN_H