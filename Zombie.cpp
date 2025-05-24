#include "Zombie.h"
#include "Plant.h" // ȷ�������� Plant.h ��ʹ�� DefensePlant
#include <cmath>
#include <random>
#include <algorithm> // ��Ҫ���� <algorithm> ��ʹ�� std::sort (�����Ҫ����) �������㷨

// ������ʬ��ʵ��
Zombie::Zombie(int init_hp, int init_attack_power, POINT init_pos, double init_speed,
    Atlas * atlas, int frame_interval, int atk_interval)
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
    target_plant = nullptr;
    target_brain = nullptr;
    attacking_brain = false;
    double min_dist_sq = -1.0; // ʹ�þ����ƽ���Ա��⿪������, -1.0 ��ʾ��δ�ҵ�Ŀ��

    Plant* current_target_plant = nullptr;

    // ���ȼ�1: Ѱ������ķ�����ֲ�� (DefensePlant �� IsPriority() ���� true)
    for (Plant* plant : plants) {
        if (plant && plant->IsAlive()) {
            if (DefensePlant* defense_plant = dynamic_cast<DefensePlant*>(plant)) {
                if (defense_plant->IsPriority()) { // ���� DefensePlant �� IsPriority() ����
                    POINT plant_pos = plant->GetPosition();
                    double dist_sq = pow(position.x - plant_pos.x, 2) + pow(position.y - plant_pos.y, 2);
                    if (current_target_plant == nullptr || dist_sq < min_dist_sq) {
                        min_dist_sq = dist_sq;
                        current_target_plant = plant;
                    }
                }
            }
        }
    }

    if (current_target_plant) {
        target_plant = current_target_plant;
        target_position = target_plant->GetPosition();
        return; // �ҵ�������ȼ�Ŀ�ֱ꣬�ӷ���
    }

    // ���ȼ�2: Ѱ���������������ֲ��
    min_dist_sq = -1.0; // ������С����
    current_target_plant = nullptr; // ���õ�ǰĿ��

    for (Plant* plant : plants) {
        if (plant && plant->IsAlive()) {
            // ȷ���������ȷ�����ֲ�� (��������Ѿ������)
            // ���߸��򵥵ķ�ʽ�ǣ���������� DefensePlant ���� IsPriority() Ϊ false
            bool is_priority_defense = false;
            if (DefensePlant* defense_plant = dynamic_cast<DefensePlant*>(plant)) {
                if (defense_plant->IsPriority()) {
                    is_priority_defense = true;
                }
            }

            if (!is_priority_defense) {
                POINT plant_pos = plant->GetPosition();
                double dist_sq = pow(position.x - plant_pos.x, 2) + pow(position.y - plant_pos.y, 2);
                if (current_target_plant == nullptr || dist_sq < min_dist_sq) {
                    min_dist_sq = dist_sq;
                    current_target_plant = plant;
                }
            }
        }
    }

    if (current_target_plant) {
        target_plant = current_target_plant;
        target_position = target_plant->GetPosition();
        return; // �ҵ�Ŀ�꣬����
    }

    // ���ȼ� 3: �������Ի���
    if (brain && brain->IsAlive() && brain->IsPlaced()) { // ȷ�������ѷ����Ҵ��
        target_plant = nullptr;
        target_brain = brain;
        attacking_brain = true;
        target_position = brain->GetPosition();
        // ����Ҫ�����룬��Ϊ����������ѡ�񣬽�ʬ��ֱ��������
    }
    else {
        // ���û��ֲ��Ҳû�л��أ���ʬ�����趨һ��Ĭ���ƶ�Ŀ�꣬������Ļ����Ե
        // ���߱��ֵ�ǰĿ��Ϊ�գ��� Update �д�����Ŀ������
        target_position = { 0, position.y }; // ʾ��������Ļ������ƶ�
    }
}

void Zombie::Update(int delta, const std::vector<Plant*>& plants, BrainBase* brain) {
    if (!is_alive) return;

    // ���Ҳ�����Ŀ��
    if ((!target_plant || !target_plant->IsAlive()) && (!target_brain || !target_brain->IsAlive())) {
        FindNearestTarget(plants, brain);
    }

    // �����ƶ�����
    double move_distance = speed * delta / 100.0;
	// deltaΪʱ�����������룩��speedΪ�ٶȣ�����/0.1�룩��move_distanceΪ�ƶ����루���أ�
    // speed*10  ����/��
    // ���û��Ŀ�꣬����Ĭ�������ƶ�
    if (!target_plant && !target_brain) {
        position.x -= static_cast<long>(move_distance);
    }
    // �����Ŀ�꣬��Ŀ���ƶ�
    else {
        // ��ȡʵ��Ŀ��λ��
        POINT actual_target = target_plant ? target_plant->GetPosition() : target_brain->GetPosition();
		// �﷨��Щ���ӣ��൱��ʵ��Ŀ����ֲ�����صĵ�ǰλ�ã�
        // ������Ŀ��ľ���ͷ���
        double dx = actual_target.x - position.x;
        double dy = actual_target.y - position.y;
        double distance = sqrt(dx * dx + dy * dy);

        // �ڹ�����Χ���򹥻�
        if (distance <= 10) {
            attack_timer += delta;
            if (attack_timer >= attack_interval) {
                Attack();
                attack_timer = 0;
            }
        }
        // ��Ҫ�ƶ�ʱ
        else {
            // �����ƶ����򣨵�λ������
            double direction_x = dx / distance;
            double direction_y = dy / distance;

            // Ӧ���ƶ�
            position.x += static_cast<long>(direction_x * move_distance);
            position.y += static_cast<long>(direction_y * move_distance);
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
        3,              // �ٶ���ߵ�100
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
        3.0,               // �ٶ���ߵ�80
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
        3.0,               // �ٶ���ߵ�60
        new Atlas(_T("img/normal_zombie_%d.png"), 22),
        new Atlas(_T("img/bucket_head_zombie_%d.png"), 14),
        100,
        1000) {
}

// ZombieSpawnerʵ��
ZombieSpawner::ZombieSpawner(const std::vector<POINT>& initial_spawn_positions, int base_interval, double chance)
    : spawn_positions(initial_spawn_positions),
    base_spawn_interval(base_interval),
	spawn_timer(0),// ��ʱ����ʼ��Ϊ0
    spawn_chance(chance) {
    std::random_device rd;
    rng.seed(rd()); // ʹ������豸��ʼ�������������

    if (spawn_positions.empty()) {
        // ���û���ṩ���ɵ㣬��������һ��Ĭ��λ�û��׳�����
        // �������Ǽ򵥵ؽ���ǰ�������Ϊ�������
        current_spawn_interval = base_spawn_interval;
    }
    else {
        int num_tombstones = spawn_positions.size();
        if (num_tombstones == 1) {
            current_spawn_interval = base_spawn_interval;
        }
        else if (num_tombstones == 2) {
            current_spawn_interval = base_spawn_interval / 2;
        }
        else { // 3�������Ĺ��
            current_spawn_interval = base_spawn_interval / 3;
        }
        // ȷ���������̫С
        if (current_spawn_interval < 100) { // ���磬��С���100ms
            current_spawn_interval = 100;
        }
    }
}

void ZombieSpawner::UpdateSpawnPositions(const std::vector<POINT>& new_spawn_positions) {
    spawn_positions = new_spawn_positions;
    if (spawn_positions.empty()) {
        current_spawn_interval = base_spawn_interval;
    }
    else {
        int num_tombstones = spawn_positions.size();
        if (num_tombstones == 1) {
            current_spawn_interval = base_spawn_interval;
        }
        else if (num_tombstones == 2) {
            current_spawn_interval = base_spawn_interval / 2;
        }
        else {
            current_spawn_interval = base_spawn_interval / 3;
        }
        if (current_spawn_interval < 100) {
            current_spawn_interval = 100;
        }
    }
}

Zombie* ZombieSpawner :: Update(int delta) {
    if (spawn_positions.empty()) {
        return nullptr; // û�����ɵ������ɽ�ʬ
    }

    spawn_timer += delta;

    if (spawn_timer >= current_spawn_interval) { // ʹ�õ�����ļ��
        spawn_timer = 0;

        std::uniform_real_distribution<> dis_chance(0.0, 1.0);
        if (dis_chance(rng) < spawn_chance) {
            // ��������������ֽ�ʬ
            std::uniform_int_distribution<> type_dis(0, 2); // 0: Normal, 1: Cone, 2: Bucket
            int zombie_type = type_dis(rng);

            // ��Ĺ���б������ѡ��һ������λ��
            std::uniform_int_distribution<> pos_dis(0, spawn_positions.size() - 1);
            POINT base_spawn_pos = spawn_positions[pos_dis(rng)];

            // ��ѡ��Ĺ��λ�ø������ƫ��һ��
            POINT spawn_pos = base_spawn_pos;
            std::uniform_int_distribution<> offset_dis(-10, 10); // ��С��ƫ�ƣ���Ϊ�ǻ���Ĺ����ȷλ��
            spawn_pos.y += offset_dis(rng);
            // ����ѡ��Ҳ��x����ƫ��
            // spawn_pos.x += offset_dis(rng);


            switch (zombie_type) {
            case 0:
                return new NormalZombie(spawn_pos);
            case 1:
                return new ConeZombie(spawn_pos);
            case 2:
                return new BucketZombie(spawn_pos);
            default:
                return new NormalZombie(spawn_pos); // Ĭ�����
            }
        }
    }
    return nullptr;
}