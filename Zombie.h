#ifndef ZOMBIE_H
#define ZOMBIE_H

#include "Animation.h"

// 基础僵尸类
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
    void SetTarget(POINT target) { target_position = target; }
};

// 普通僵尸类
class NormalZombie : public Zombie {
public:
    NormalZombie(POINT init_pos);
};

// 有防具僵尸基类
class ArmoredZombie : public Zombie {
protected:
    int armor_hp;           // 防具血量
    bool has_armor;         // 是否有防具
    Atlas* armor_atlas;     // 带防具时的动画集
    Animation* armor_anim;  // 带防具时的动画

public:
    ArmoredZombie(int init_hp, int armor_init_hp, POINT init_pos, double init_speed,
                  Atlas* zombie_atlas, Atlas* armor_atlas,
                  int frame_interval, int atk_interval);
    virtual ~ArmoredZombie();
    virtual void TakeDamage(int damage) override;
    virtual void Draw() override;
    bool HasArmor() const { return has_armor; }
};

// 路障僵尸类
class ConeZombie : public ArmoredZombie {
public:
    ConeZombie(POINT init_pos);
};

// 铁桶僵尸类
class BucketZombie : public ArmoredZombie {
public:
    BucketZombie(POINT init_pos);
};

#endif // ZOMBIE_H