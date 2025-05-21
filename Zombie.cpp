#include "Zombie.h"
#include <cmath>
#include <random>

// ������ʬ��ʵ��
Zombie::Zombie(int init_hp, int init_attack_power, POINT init_pos, double init_speed,
    Atlas* atlas, int frame_interval, int atk_interval)
    : hp(init_hp), attack_power(init_attack_power), position(init_pos),
    speed(init_speed), is_alive(true), attack_interval(atk_interval), attack_timer(0),
    target_plant(nullptr), target_brain(nullptr), attacking_brain(false) {
    anim = new Animation(atlas, frame_interval);
    // Ĭ�������ƶ�
    target_position = { 0, init_pos.y };
}

Zombie::~Zombie() {
    if (anim) {
        delete anim;
    }
}

double Zombie::CalculateDistance(POINT p1, POINT p2) const {
    int dx = p1.x - p2.x;
    int dy = p1.y - p2.y;
    return sqrt(dx * dx + dy * dy);
}

void Zombie::FindNearestTarget(const std::vector<Plant*>& plants, BrainBase* brain) {
    double min_distance = 1e9;
    Plant* nearest_defense = nullptr;
    Plant* nearest_other = nullptr;

    // ����Ѱ������ķ�����ֲ�������ֲ��
    for (Plant* plant : plants) {
        if (!plant->IsAlive()) continue;

        POINT plant_pos = plant->GetPosition();
        double dist = CalculateDistance(position, plant_pos);

        if (DefensePlant* defense = dynamic_cast<DefensePlant*>(plant)) {
            if (dist < min_distance) {
                min_distance = dist;
                nearest_defense = plant;
            }
        }
        else {
            if (dist < min_distance) {
                min_distance = dist;
                nearest_other = plant;
            }
        }
    }

    // ����ѡ�������ֲ��
    if (nearest_defense) {
        target_plant = nearest_defense;
        target_brain = nullptr;
        attacking_brain = false;
        target_position = target_plant->GetPosition();
    }
    // ���ѡ������ֲ��
    else if (nearest_other) {
        target_plant = nearest_other;
        target_brain = nullptr;
        attacking_brain = false;
        target_position = target_plant->GetPosition();
    }
    // ���ѡ����Ի���
    else if (brain && brain->IsAlive()) {
        target_plant = nullptr;
        target_brain = brain;
        attacking_brain = true;
        target_position = brain->GetPosition();
    }
}

void Zombie::Update(int delta, const std::vector<Plant*>& plants, BrainBase* brain) {
    if (!is_alive) return;

    // ���Ҳ�����Ŀ��
    if ((!target_plant || !target_plant->IsAlive()) && (!target_brain || !target_brain->IsAlive())) {
        FindNearestTarget(plants, brain);
    }

    // �����ƶ�����
    double move_distance = speed * delta / 1000.0;

    // ���û��Ŀ�ֱ꣬�������ƶ�
    if (!target_plant && !target_brain) {
        position.x -= static_cast<long>(move_distance);
    }
    // �����Ŀ�꣬��Ŀ���ƶ�
    else {
        double dx = target_position.x - position.x;
        double dy = target_position.y - position.y;
        double distance = sqrt(dx * dx + dy * dy);

        if (distance <= 50) {  // �ڹ�����Χ��
            attack_timer += delta;
            if (attack_timer >= attack_interval) {
                Attack();
                attack_timer = 0;
            }
        }
        else {  // ��Ŀ���ƶ�
            position.x -= static_cast<long>(move_distance);  // ʼ�������ƶ�
            
            // ����Y��λ��
            if (abs(dy) > 5) {
                if (dy > 0) {
                    position.y += static_cast<long>(move_distance / 2);  // Y���ƶ��ٶȼ���
                } else {
                    position.y -= static_cast<long>(move_distance / 2);
                }
            }
        }
    }

    // ȷ�������Ƴ���Ļ�߽�
    if (position.x < 0) position.x = 0;
    if (position.y < 0) position.y = 0;
    if (position.y > 720 - ZOMBIE_HEIGHT) position.y = 720 - ZOMBIE_HEIGHT;
}

void Zombie::Attack() {
    if (target_plant && target_plant->IsAlive()) {
        target_plant->TakeDamage(attack_power);
    }
    else if (target_brain && target_brain->IsAlive()) {
        target_brain->TakeDamage(attack_power);
    }
}

void Zombie::TakeDamage(int damage) {
    if (!is_alive) return;
    hp -= damage;
    if (hp <= 0) {
        hp = 0;
        is_alive = false;
    }
}

void Zombie::Draw() {
    if (is_alive && anim) {
        anim->showimage(position.x, position.y, 1000 / 144);
    }
}

// ��ͨ��ʬʵ��
NormalZombie::NormalZombie(POINT init_pos)
    : Zombie(100,                // ����ֵ
            10,                  // ������
            init_pos,
            100.0,              // �ٶ���ߵ�100
            new Atlas(_T("img/normal_zombie_%d.png"), 22),
            100,
            1000) {
}

// �з��߽�ʬ����ʵ��
ArmoredZombie::ArmoredZombie(int init_hp, int armor_init_hp, POINT init_pos, double init_speed,
    Atlas* zombie_atlas, Atlas* armor_atlas,
    int frame_interval, int atk_interval)
    : Zombie(init_hp, 10, init_pos, init_speed, zombie_atlas, frame_interval, atk_interval),
    armor_hp(armor_init_hp), has_armor(true) {
    this->armor_atlas = armor_atlas;
    armor_anim = new Animation(armor_atlas, frame_interval);
}

ArmoredZombie::~ArmoredZombie() {
    if (armor_anim) {
        delete armor_anim;
    }
    if (armor_atlas) {
        delete armor_atlas;
    }
}

void ArmoredZombie::TakeDamage(int damage) {
    if (!is_alive) return;

    if (has_armor) {
        // ���������ܵ��˺�
        armor_hp -= damage;
        if (armor_hp <= 0) {
            has_armor = false;  // ���߱��ƻ�
        }
    }
    else {
        // û�з���ʱ��ֱ���˺���ʬ����
        Zombie::TakeDamage(damage);
    }
}

void ArmoredZombie::Draw() {
    if (!is_alive) return;

    if (has_armor && armor_anim) {
        // �з���ʱ��ʾ�����ߵĶ���
        armor_anim->showimage(position.x, position.y, 1000 / 144);
    }
    else {
        // û�з���ʱ��ʾ��ͨ��ʬ����
        Zombie::Draw();
    }
}

// ·�Ͻ�ʬʵ��
ConeZombie::ConeZombie(POINT init_pos)
    : ArmoredZombie(100,
        80,
        init_pos,
        80.0,               // �ٶ���ߵ�80
        new Atlas(_T("img/normal_zombie_%d.png"), 22),
        new Atlas(_T("img/cone_head_zombie_%d.png"), 20),
        100,
        1000) {
}

// ��Ͱ��ʬʵ��
BucketZombie::BucketZombie(POINT init_pos)
    : ArmoredZombie(100,
        150,
        init_pos,
        60.0,               // �ٶ���ߵ�60
        new Atlas(_T("img/normal_zombie_%d.png"), 22),
        new Atlas(_T("img/bucket_head_zombie_%d.png"), 14),
        100,
        1000) {
}

// ZombieSpawnerʵ��
ZombieSpawner::ZombieSpawner(POINT pos, int interval, double chance)
    : spawn_position(pos), spawn_interval(interval),
    spawn_timer(0), spawn_chance(chance) {
}

Zombie* ZombieSpawner::Update(int delta) {
    spawn_timer += delta;

    if (spawn_timer >= spawn_interval) {
        spawn_timer = 0;

        // ��������Ƿ����ɽ�ʬ
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0, 1);

        if (dis(gen) < spawn_chance) {
            // ��������������ֽ�ʬ
            std::uniform_int_distribution<> type_dis(0, 2);
            int zombie_type = type_dis(gen);

            // ������λ�ø������ƫ��һ�㣬ʹ��ʬ������ȫ�ص�
            POINT spawn_pos = spawn_position;
            std::uniform_int_distribution<> offset_dis(-50, 50);
            spawn_pos.y += offset_dis(gen);

            switch (zombie_type) {
            case 0:
                return new NormalZombie(spawn_pos);
            case 1:
                return new ConeZombie(spawn_pos);
            case 2:
                return new BucketZombie(spawn_pos);
            default:
                return new NormalZombie(spawn_pos);
            }
        }
    }

    return nullptr;
}