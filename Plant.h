#ifndef PLANT_H
#define PLANT_H

#include "Animation.h"
#include <vector>
// 前向声明
class Zombie;
class Bullet;

class Plant {
protected:
    int hp;
    int max_hp; // 存储最大生命值
    POINT position;
    int cost;
    Animation* anim;
    bool is_alive;

public:
    Plant(int init_hp, POINT pos, int init_cost, Atlas* atlas, int frame_interval);
    virtual ~Plant();
    
    //virtual void Update(int delta, std::vector<Zombie*>& zombies, std::vector<Bullet*>& bullets) = 0; // 纯虚函数，后续覆盖
    
    virtual void Update(int delta) = 0;// 重载纯虚函数，无需僵尸锁敌的植物不需要上述函数
    
    int GetHP() const;
    int GetMaxHP() const; // 获取最大生命值
    POINT GetPosition() const;
    int GetCost() const;
    bool IsAlive() const;
    virtual void TakeDamage(int damage);
    virtual void Draw();
    virtual void IncreaseMaxHPAndHeal(int amount) {
               max_hp += amount;
               hp += amount;
               if (hp > max_hp) hp = max_hp; // 确保当前HP不超过新的最大HP
    }
};

class AttackPlant : public Plant {
protected:
    int attack_power;    // 攻击力 (子弹造成的伤害)
    int attack_range;    // 攻击范围 (例如，多少像素远)
    int attack_interval; // 攻击间隔 (毫秒)
    int attack_timer;    // 攻击计时器

public:
    AttackPlant(int init_hp, POINT pos, int init_cost, Atlas* atlas,
        int frame_interval, int power, int range, int interval);

    // AttackPlant 的更新逻辑，处理攻击计时和调用 Attack
    void Update(int delta) override;
    
    virtual void UpdateAttackLogic(int delta, std::vector<Zombie*>& zombies, std::vector<Bullet*>& bullets);

    virtual void Attack(Zombie* target_zombie, std::vector<Bullet*>& bullets) = 0; // 修改参数
    
    void IncreaseAttackPower(int amount) { attack_power += amount; }

};

class DefensePlant : public Plant {
protected:
    int defense;
    bool is_priority;

public:
    DefensePlant(int init_hp, POINT pos, int init_cost, Atlas* atlas,
        int frame_interval, int init_defense, bool priority);
    void Update(int delta) override;
    virtual void TakeDamage(int damage) override;
    bool IsPriority() const;
};

class ResourcePlant : public Plant {
protected:
    int resource_rate;   // 产生资源的速率 (例如，每隔多少毫秒)
    int resource_amount; // 一次产生多少资源 (例如，阳光数量)
    int resource_timer;  // 资源产生计时器

public:
    ResourcePlant(int init_hp, POINT pos, int init_cost, Atlas* atlas,
        int frame_interval, int rate, int amount);
    void Update(int delta) override;
    // 为什么删掉virtual就不报错了？
    
    virtual void GenerateResource() = 0;
};

#endif // PLANT_H