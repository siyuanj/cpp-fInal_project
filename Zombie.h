#ifndef ZOMBIE_H
#define ZOMBIE_H

#include "Animation.h"
#include "Plant.h"
#include "BrainBase.h"
#include <vector>

// 前向声明
class Plant;
class BrainBase;

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

    // 攻击目标
    Plant* target_plant;
    BrainBase* target_brain;
    bool attacking_brain;

    // 查找最近的目标
    void FindNearestTarget(const std::vector<Plant*>& plants, BrainBase* brain);
    double CalculateDistance(POINT p1, POINT p2) const;

public:
    static const int ZOMBIE_WIDTH = 166;
    static const int ZOMBIE_HEIGHT = 144;

    Zombie(int init_hp, int init_attack_power, POINT init_pos, double init_speed,
           Atlas* atlas, int frame_interval, int atk_interval);
    virtual ~Zombie();
    virtual void Update(int delta, const std::vector<Plant*>& plants, BrainBase* brain);
    virtual void Draw();
    virtual void TakeDamage(int damage);
    virtual void Attack();

    // Getter函数
    bool IsAlive() const { return is_alive; }
    POINT GetPosition() const { return position; }
    int GetAttackPower() const { return attack_power; }
    double GetSpeed() const { return speed; }  // 添加获取速度的函数
    POINT GetTargetPosition() const { return target_position; }  // 添加获取目标位置的函数
    void SetTarget(POINT target) { target_position = target; }
};

// ZombieSpawner类 - 负责生成僵尸
class ZombieSpawner {
private:
    POINT spawn_position;    // 僵尸生成位置（墓碑位置）
    int spawn_interval;      // 生成间隔（毫秒）
    int spawn_timer;         // 生成计时器
    double spawn_chance;     // 生成概率

public:
    ZombieSpawner(POINT pos, int interval = 1000, double chance = 0.3);

    // 更新并可能生成新僵尸
    Zombie* Update(int delta);

    // 设置生成参数
    void SetSpawnInterval(int interval) { spawn_interval = interval; }
    void SetSpawnChance(double chance) { spawn_chance = chance; }
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