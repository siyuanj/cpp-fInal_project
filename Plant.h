#ifndef PLANT_H
#define PLANT_H

#include "Animation.h"
#include <vector>

class Plant {
protected:
    int hp;
    POINT position;
    int cost;
    Animation* anim;
    bool is_alive;

public:
    Plant(int init_hp, POINT pos, int init_cost, Atlas* atlas, int frame_interval);
    virtual ~Plant();
    virtual void Update(int delta) = 0;
    int GetHP() const;
    POINT GetPosition() const;
    int GetCost() const;
    bool IsAlive() const;
    virtual void TakeDamage(int damage);
    virtual void Draw();
};

class AttackPlant : public Plant {
protected:
    int attack_power;
    int attack_range;
    int attack_interval;
    int attack_timer;

public:
    AttackPlant(int init_hp, POINT pos, int init_cost, Atlas* atlas,
        int frame_interval, int power, int range, int interval);
    virtual void Update(int delta) override;
    virtual void Attack() = 0;
};

class DefensePlant : public Plant {
protected:
    int defense;
    bool is_priority;

public:
    DefensePlant(int init_hp, POINT pos, int init_cost, Atlas* atlas,
        int frame_interval, int init_defense, bool priority);
    virtual void Update(int delta) override;
    virtual void TakeDamage(int damage) override;
    bool IsPriority() const;
};

class ResourcePlant : public Plant {
protected:
    int resource_rate;
    int resource_amount;
    int resource_timer;

public:
    ResourcePlant(int init_hp, POINT pos, int init_cost, Atlas* atlas,
        int frame_interval, int rate, int amount);
    void Update(int delta) override;
    virtual void GenerateResource() = 0;
};

#endif // PLANT_H