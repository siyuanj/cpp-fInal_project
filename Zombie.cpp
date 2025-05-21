#include "Zombie.h"
#include <cmath>
#include <random>

// 基础僵尸类实现
Zombie::Zombie(int init_hp, int init_attack_power, POINT init_pos, double init_speed,
    Atlas* atlas, int frame_interval, int atk_interval)
    : hp(init_hp), attack_power(init_attack_power), position(init_pos),
    speed(init_speed), is_alive(true), attack_interval(atk_interval), attack_timer(0),
    target_plant(nullptr), target_brain(nullptr), attacking_brain(false) {
    anim = new Animation(atlas, frame_interval);
    // 默认向左移动
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

    // 首先寻找最近的防御型植物和其他植物
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

    // 优先选择防御型植物
    if (nearest_defense) {
        target_plant = nearest_defense;
        target_brain = nullptr;
        attacking_brain = false;
        target_position = target_plant->GetPosition();
    }
    // 其次选择其他植物
    else if (nearest_other) {
        target_plant = nearest_other;
        target_brain = nullptr;
        attacking_brain = false;
        target_position = target_plant->GetPosition();
    }
    // 最后选择大脑基地
    else if (brain && brain->IsAlive()) {
        target_plant = nullptr;
        target_brain = brain;
        attacking_brain = true;
        target_position = brain->GetPosition();
    }
}

void Zombie::Update(int delta, const std::vector<Plant*>& plants, BrainBase* brain) {
    if (!is_alive) return;

    // 查找并更新目标
    if ((!target_plant || !target_plant->IsAlive()) && (!target_brain || !target_brain->IsAlive())) {
        FindNearestTarget(plants, brain);
    }

    // 计算移动距离
    double move_distance = speed * delta / 1000.0;

    // 如果没有目标，直接向左移动
    if (!target_plant && !target_brain) {
        position.x -= static_cast<long>(move_distance);
    }
    // 如果有目标，向目标移动
    else {
        double dx = target_position.x - position.x;
        double dy = target_position.y - position.y;
        double distance = sqrt(dx * dx + dy * dy);

        if (distance <= 50) {  // 在攻击范围内
            attack_timer += delta;
            if (attack_timer >= attack_interval) {
                Attack();
                attack_timer = 0;
            }
        }
        else {  // 向目标移动
            position.x -= static_cast<long>(move_distance);  // 始终向左移动
            
            // 调整Y轴位置
            if (abs(dy) > 5) {
                if (dy > 0) {
                    position.y += static_cast<long>(move_distance / 2);  // Y轴移动速度减半
                } else {
                    position.y -= static_cast<long>(move_distance / 2);
                }
            }
        }
    }

    // 确保不会移出屏幕边界
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

// 普通僵尸实现
NormalZombie::NormalZombie(POINT init_pos)
    : Zombie(100,                // 生命值
            10,                  // 攻击力
            init_pos,
            100.0,              // 速度提高到100
            new Atlas(_T("img/normal_zombie_%d.png"), 22),
            100,
            1000) {
}

// 有防具僵尸基类实现
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
        // 防具优先受到伤害
        armor_hp -= damage;
        if (armor_hp <= 0) {
            has_armor = false;  // 防具被破坏
        }
    }
    else {
        // 没有防具时，直接伤害僵尸本体
        Zombie::TakeDamage(damage);
    }
}

void ArmoredZombie::Draw() {
    if (!is_alive) return;

    if (has_armor && armor_anim) {
        // 有防具时显示带防具的动画
        armor_anim->showimage(position.x, position.y, 1000 / 144);
    }
    else {
        // 没有防具时显示普通僵尸动画
        Zombie::Draw();
    }
}

// 路障僵尸实现
ConeZombie::ConeZombie(POINT init_pos)
    : ArmoredZombie(100,
        80,
        init_pos,
        80.0,               // 速度提高到80
        new Atlas(_T("img/normal_zombie_%d.png"), 22),
        new Atlas(_T("img/cone_head_zombie_%d.png"), 20),
        100,
        1000) {
}

// 铁桶僵尸实现
BucketZombie::BucketZombie(POINT init_pos)
    : ArmoredZombie(100,
        150,
        init_pos,
        60.0,               // 速度提高到60
        new Atlas(_T("img/normal_zombie_%d.png"), 22),
        new Atlas(_T("img/bucket_head_zombie_%d.png"), 14),
        100,
        1000) {
}

// ZombieSpawner实现
ZombieSpawner::ZombieSpawner(POINT pos, int interval, double chance)
    : spawn_position(pos), spawn_interval(interval),
    spawn_timer(0), spawn_chance(chance) {
}

Zombie* ZombieSpawner::Update(int delta) {
    spawn_timer += delta;

    if (spawn_timer >= spawn_interval) {
        spawn_timer = 0;

        // 随机决定是否生成僵尸
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0, 1);

        if (dis(gen) < spawn_chance) {
            // 随机决定生成哪种僵尸
            std::uniform_int_distribution<> type_dis(0, 2);
            int zombie_type = type_dis(gen);

            // 在生成位置附近随机偏移一点，使僵尸不会完全重叠
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