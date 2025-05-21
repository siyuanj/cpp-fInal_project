#ifndef ZOMBIE_H
#define ZOMBIE_H

#include "Animation.h"

class Zombie {
protected:
    Animation* anim;
    int hp;
    int attack_power;
    POINT position;
    POINT target_position;
    double speed;
    bool is_alive;
    int attack_interval;
    int attack_timer;

public:
    Zombie(int init_hp, int init_attack_power, POINT init_pos, double init_speed,
           Atlas* atlas, int frame_interval, int atk_interval);
    virtual ~Zombie();
    virtual void Update(int delta);
    virtual void Draw();
    virtual void TakeDamage(int damage);
    bool IsAlive() const;
    POINT GetPosition() const;
    int GetAttackPower() const;
};

#endif // ZOMBIE_H