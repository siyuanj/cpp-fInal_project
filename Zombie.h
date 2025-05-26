#ifndef ZOMBIE_H
#define ZOMBIE_H

#include "Animation.h"
#include "Plant.h"
#include "BrainBase.h"
#include <vector>
#include <random> // 需要包含 <random> 以使用 std::mt19937 和 std::uniform_int_distribution

// 前向声明
class Plant;
class BrainBase;

// 基础僵尸类
class Zombie {
protected:
    Animation* anim;
    int hp;
    int max_hp;
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
    std::vector<POINT> spawn_positions; // 修改为存储多个墓碑位置
    int base_spawn_interval;          // 基础生成间隔
    int current_spawn_interval;       // 当前实际的生成间隔
    int spawn_timer;                  // 生成计时器
    double spawn_chance;              // 生成概率
    std::mt19937 rng;                 // 随机数生成器

public:
    ZombieSpawner(const std::vector<POINT>& initial_spawn_positions, int base_interval = 1000, double chance = 0.3);

    // 更新墓碑位置
    void UpdateSpawnPositions(const std::vector<POINT>& new_spawn_positions);

    // 更新并可能生成新僵尸
    Zombie* Update(int delta);
	// 用于修改生成概率
    void SetSpawnChance(double chance) { spawn_chance = chance; }
};
    


// 普通僵尸类
class NormalZombie : public Zombie {
public:
    NormalZombie(POINT init_pos);
};

// 精英僵尸
class EliteZombie : public Zombie {
public:
    EliteZombie(POINT init_pos);
};

// 有防具僵尸基类
class ArmoredZombie : public Zombie {
protected:
    int armor_hp;           // 防具血量
    int max_armor_hp;
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