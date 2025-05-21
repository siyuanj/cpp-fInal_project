#include "Sun.h"
#include <cmath>

Sun::Sun(POINT pos, Atlas* atlas) : position(pos), is_active(true) {
    target = { 68, 2 };
    anim = new Animation(atlas, 50);
}

Sun::~Sun() {
    if (anim) delete anim;
}

void Sun::Update(int delta) {
    if (!is_active) return;

    double dx = target.x - position.x;
    double dy = target.y - position.y;
    double distance = sqrt(dx * dx + dy * dy);

    if (distance < MOVE_SPEED) {
        is_active = false;
        sun_count += 25;
    }
    else {
        position.x += (dx / distance) * MOVE_SPEED;
        position.y += (dy / distance) * MOVE_SPEED;
    }
}

void Sun::Draw() {
    if (is_active && anim) {
        anim->showimage(position.x, position.y, 1000 / 144);
    }
}

bool Sun::IsActive() const { return is_active; }