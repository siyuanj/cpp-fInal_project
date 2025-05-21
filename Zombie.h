#ifndef ZOMBIE_H
#define ZOMBIE_H

#include "Animation.h"

// ������ʬ��
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

// ��ͨ��ʬ��
class NormalZombie : public Zombie {
public:
    NormalZombie(POINT init_pos);
};

// �з��߽�ʬ����
class ArmoredZombie : public Zombie {
protected:
    int armor_hp;           // ����Ѫ��
    bool has_armor;         // �Ƿ��з���
    Atlas* armor_atlas;     // ������ʱ�Ķ�����
    Animation* armor_anim;  // ������ʱ�Ķ���

public:
    ArmoredZombie(int init_hp, int armor_init_hp, POINT init_pos, double init_speed,
                  Atlas* zombie_atlas, Atlas* armor_atlas,
                  int frame_interval, int atk_interval);
    virtual ~ArmoredZombie();
    virtual void TakeDamage(int damage) override;
    virtual void Draw() override;
    bool HasArmor() const { return has_armor; }
};

// ·�Ͻ�ʬ��
class ConeZombie : public ArmoredZombie {
public:
    ConeZombie(POINT init_pos);
};

// ��Ͱ��ʬ��
class BucketZombie : public ArmoredZombie {
public:
    BucketZombie(POINT init_pos);
};

#endif // ZOMBIE_H