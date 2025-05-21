#include "Bullet.h"

// Initialize static member
IMAGE* NormalBullet::bullet_img = nullptr;

Bullet::Bullet(POINT pos, double init_speed)
    : position(pos), speed(init_speed), is_active(true) {
    anim = nullptr;
}

Bullet::~Bullet() {
    if (anim) delete anim;
}

void Bullet::Update(int delta) {
    if (!is_active) return;

    position.x += speed * delta / 1000.0;

    if (position.x > 1280) {
        is_active = false;
    }
}

void Bullet::Draw() {
    if (is_active && anim) {
        anim->showimage(position.x, position.y, 1000 / 144);
    }
}

POINT Bullet::GetPosition() const { return position; }
bool Bullet::IsActive() const { return is_active; }
void Bullet::Deactivate() { is_active = false; }

NormalBullet::NormalBullet(POINT pos, double init_speed)
    : Bullet(pos, init_speed) {
    if (!bullet_img) {
        bullet_img = new IMAGE();
        loadimage(bullet_img, _T("img/normal_bullet.gif"));
    }
}

void NormalBullet::Draw() {
    if (IsActive() && bullet_img) {
        putimage_alpha(GetPosition().x, GetPosition().y, bullet_img);
    }
}

void NormalBullet::Cleanup() {
    if (bullet_img) {
        delete bullet_img;
        bullet_img = nullptr;
    }
}