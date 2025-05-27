#ifndef PLANT_H
#define PLANT_H

#include "Animation.h"
#include <vector>
// ǰ������
class Zombie;
class Bullet;

class Plant {
protected:
    int hp;
    int max_hp; // �洢�������ֵ
    POINT position;
    int cost;
    Animation* anim;
    bool is_alive;

public:
    Plant(int init_hp, POINT pos, int init_cost, Atlas* atlas, int frame_interval);
    virtual ~Plant();
    
    //virtual void Update(int delta, std::vector<Zombie*>& zombies, std::vector<Bullet*>& bullets) = 0; // ���麯������������
    
    virtual void Update(int delta) = 0;// ���ش��麯�������轩ʬ���е�ֲ�ﲻ��Ҫ��������
    
    int GetHP() const;
    int GetMaxHP() const; // ��ȡ�������ֵ
    POINT GetPosition() const;
    int GetCost() const;
    bool IsAlive() const;
    virtual void TakeDamage(int damage);
    virtual void Draw();
    virtual void IncreaseMaxHPAndHeal(int amount) {
               max_hp += amount;
               hp += amount;
               if (hp > max_hp) hp = max_hp; // ȷ����ǰHP�������µ����HP
    }
};

class AttackPlant : public Plant {
protected:
    int attack_power;    // ������ (�ӵ���ɵ��˺�)
    int attack_range;    // ������Χ (���磬��������Զ)
    int attack_interval; // ������� (����)
    int attack_timer;    // ������ʱ��

public:
    AttackPlant(int init_hp, POINT pos, int init_cost, Atlas* atlas,
        int frame_interval, int power, int range, int interval);

    // AttackPlant �ĸ����߼�����������ʱ�͵��� Attack
    void Update(int delta) override;
    
    virtual void UpdateAttackLogic(int delta, std::vector<Zombie*>& zombies, std::vector<Bullet*>& bullets);

    virtual void Attack(Zombie* target_zombie, std::vector<Bullet*>& bullets) = 0; // �޸Ĳ���
    
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
    int resource_rate;   // ������Դ������ (���磬ÿ�����ٺ���)
    int resource_amount; // һ�β���������Դ (���磬��������)
    int resource_timer;  // ��Դ������ʱ��

public:
    ResourcePlant(int init_hp, POINT pos, int init_cost, Atlas* atlas,
        int frame_interval, int rate, int amount);
    void Update(int delta) override;
    // Ϊʲôɾ��virtual�Ͳ������ˣ�
    
    virtual void GenerateResource() = 0;
};

#endif // PLANT_H