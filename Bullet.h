#ifndef BULLET_H
#define BULLET_H

#include "Animation.h"

class Bullet {
private:
    Animation* anim;
    double speed;
    POINT position;
    bool is_active;

public:
    Bullet(POINT pos, double init_speed);
    virtual ~Bullet();
    virtual void Update(int delta);
    virtual void Draw();
    POINT GetPosition() const;
    bool IsActive() const;
    void Deactivate();
};

class NormalBullet : public Bullet {
private:
    static IMAGE* bullet_img;

public:
    NormalBullet(POINT pos, double init_speed);
    void Draw() override;
    static void Cleanup();
};

#endif // BULLET_H