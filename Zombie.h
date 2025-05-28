#ifndef ZOMBIE_H
#define ZOMBIE_H

#include "Animation.h"
#include "Plant.h"
#include "BrainBase.h"
#include <vector>
#include <random> // ��Ҫ���� <random> ��ʹ�� std::mt19937 �� std::uniform_int_distribution

// ǰ������
class Plant;
class BrainBase;

// ������ʬ��
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


    // ����Ŀ��
    Plant* target_plant;
    BrainBase* target_brain;
    bool attacking_brain;


    // ���������Ŀ��
    void FindNearestTarget(const std::vector<Plant*>& plants, BrainBase* brain);
    double CalculateDistance(POINT p1, POINT p2) const;

public:
    static const int ZOMBIE_WIDTH = 166;
    static const int ZOMBIE_HEIGHT = 144;

    Zombie(int init_hp, int init_attack_power, POINT init_pos, double init_speed,
            int atk_interval);
    virtual ~Zombie();
    virtual void Update(int delta, const std::vector<Plant*>& plants, BrainBase* brain);
    virtual void Draw();
    virtual void TakeDamage(int damage);
    virtual void Attack();

    static void LoadResources();    // һ��ͳһ�ļ��غ���
    static void UnloadResources();  // һ��ͳһ��ж�غ���

    // Getter����
    bool IsAlive() const { return is_alive; }
    POINT GetPosition() const { return position; }
    int GetAttackPower() const { return attack_power; }
    double GetSpeed() const { return speed; }  // ��ӻ�ȡ�ٶȵĺ���
    POINT GetTargetPosition() const { return target_position; }  // ��ӻ�ȡĿ��λ�õĺ���
    void SetTarget(POINT target) { target_position = target; }
protected:
    static Atlas* atlas_normal_zombie;
    static Atlas* atlas_elite_zombie;
    static Atlas* atlas_cone_head_armor; // ·��ͷ����ͼ��
    static Atlas* atlas_bucket_head_armor; // ��Ͱͷ����ͼ��

};

// ZombieSpawner�� - �������ɽ�ʬ
class ZombieSpawner {
private:
    std::vector<POINT> spawn_positions; // �޸�Ϊ�洢���Ĺ��λ��
    int base_spawn_interval;          // �������ɼ��
    int current_spawn_interval;       // ��ǰʵ�ʵ����ɼ��
    int spawn_timer;                  // ���ɼ�ʱ��
    double spawn_chance;              // ���ɸ���
    std::mt19937 rng;                 // �����������
    


public:
    ZombieSpawner(const std::vector<POINT>& initial_spawn_positions, int base_interval = 1000, double chance = 0.3);

    // ����Ĺ��λ��
    void UpdateSpawnPositions(const std::vector<POINT>& new_spawn_positions);

    // ���²����������½�ʬ
    Zombie* Update(int delta);
	// �����޸����ɸ���
    void SetSpawnChance(double chance) { spawn_chance = chance; }
};
    


// ��ͨ��ʬ��
class NormalZombie : public Zombie {
public:
    NormalZombie(POINT init_pos);
};

// ��Ӣ��ʬ
class EliteZombie : public Zombie {
public:
    EliteZombie(POINT init_pos);
};

// �з��߽�ʬ����
class ArmoredZombie : public Zombie {
protected:
    int armor_hp;           // ����Ѫ��
    int max_armor_hp;
    bool has_armor;         // �Ƿ��з���
    //Atlas* armor_atlas;     // ������ʱ�Ķ�����
    Animation* armor_anim;  // ������ʱ�Ķ���

public:
    ArmoredZombie(int init_hp, int armor_init_hp, 
        POINT init_pos, double init_speed,
        int atk_interval, Atlas* no_armor_atlas, 
        Atlas* armor_atlas_param, int frame_interval);
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