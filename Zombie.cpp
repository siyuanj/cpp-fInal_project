#include "Zombie.h"
#include "Plant.h" // 确保包含了 Plant.h 以使用 DefensePlant
#include <cmath>
#include <random>
#include <algorithm> // 需要包含 <algorithm> 以使用 std::sort (如果需要排序) 或其他算法

// 基础僵尸类实现
Zombie::Zombie(int init_hp, int init_attack_power, POINT init_pos, double init_speed,
    Atlas * atlas, int frame_interval, int atk_interval)
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
    target_plant = nullptr;
    target_brain = nullptr;
    attacking_brain = false;
    double min_dist_sq = -1.0; // 使用距离的平方以避免开方运算, -1.0 表示尚未找到目标

    Plant* current_target_plant = nullptr;

    // 优先级1: 寻找最近的防御型植物 (DefensePlant 且 IsPriority() 返回 true)
    for (Plant* plant : plants) {
        if (plant && plant->IsAlive()) {
            if (DefensePlant* defense_plant = dynamic_cast<DefensePlant*>(plant)) {
                if (defense_plant->IsPriority()) { // 假设 DefensePlant 有 IsPriority() 方法
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
        return; // 找到最高优先级目标，直接返回
    }

    // 优先级2: 寻找最近的其他类型植物
    min_dist_sq = -1.0; // 重置最小距离
    current_target_plant = nullptr; // 重置当前目标

    for (Plant* plant : plants) {
        if (plant && plant->IsAlive()) {
            // 确保不是优先防御型植物 (如果上面已经处理过)
            // 或者更简单的方式是，如果它不是 DefensePlant 或者 IsPriority() 为 false
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
        return; // 找到目标，返回
    }

    // 优先级 3: 攻击大脑基地
    if (brain && brain->IsAlive() && brain->IsPlaced()) { // 确保基地已放置且存活
        target_plant = nullptr;
        target_brain = brain;
        attacking_brain = true;
        target_position = brain->GetPosition();
        // 不需要检查距离，因为基地是最后的选择，僵尸会直接走向它
    }
    else {
        // 如果没有植物也没有基地，僵尸可以设定一个默认移动目标，例如屏幕左侧边缘
        // 或者保持当前目标为空，在 Update 中处理无目标的情况
        target_position = { 0, position.y }; // 示例：向屏幕最左侧移动
    }
}

void Zombie::Update(int delta, const std::vector<Plant*>& plants, BrainBase* brain) {
    if (!is_alive) return;

    // 查找并更新目标
    if ((!target_plant || !target_plant->IsAlive()) && (!target_brain || !target_brain->IsAlive())) {
        FindNearestTarget(plants, brain);
    }

    // 计算移动距离
    double move_distance = speed * delta / 100.0;
	// delta为时间增量（毫秒），speed为速度（像素/0.1秒），move_distance为移动距离（像素）
    // speed*10  像素/秒
    // 如果没有目标，保持默认向左移动
    if (!target_plant && !target_brain) {
        position.x -= static_cast<long>(move_distance);
    }
    // 如果有目标，向目标移动
    else {
        // 获取实际目标位置
        POINT actual_target = target_plant ? target_plant->GetPosition() : target_brain->GetPosition();
		// 语法有些复杂，相当于实际目标是植物或基地的当前位置？
        // 计算与目标的距离和方向
        double dx = actual_target.x - position.x;
        double dy = actual_target.y - position.y;
        double distance = sqrt(dx * dx + dy * dy);

        // 在攻击范围内则攻击
        if (distance <= 10) {
            attack_timer += delta;
            if (attack_timer >= attack_interval) {
                Attack();
                attack_timer = 0;
            }
        }
        // 需要移动时
        else {
            // 计算移动方向（单位向量）
            double direction_x = dx / distance;
            double direction_y = dy / distance;

            // 应用移动
            position.x += static_cast<long>(direction_x * move_distance);
            position.y += static_cast<long>(direction_y * move_distance);
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
        3,              // 速度提高到100
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
        3.0,               // 速度提高到80
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
        3.0,               // 速度提高到60
        new Atlas(_T("img/normal_zombie_%d.png"), 22),
        new Atlas(_T("img/bucket_head_zombie_%d.png"), 14),
        100,
        1000) {
}

// ZombieSpawner实现
ZombieSpawner::ZombieSpawner(const std::vector<POINT>& initial_spawn_positions, int base_interval, double chance)
    : spawn_positions(initial_spawn_positions),
    base_spawn_interval(base_interval),
	spawn_timer(0),// 计时器初始化为0
    spawn_chance(chance) {
    std::random_device rd;
    rng.seed(rd()); // 使用随机设备初始化随机数生成器

    if (spawn_positions.empty()) {
        // 如果没有提供生成点，可以设置一个默认位置或抛出错误
        // 这里我们简单地将当前间隔设置为基础间隔
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
        else { // 3个或更多墓碑
            current_spawn_interval = base_spawn_interval / 3;
        }
        // 确保间隔不会太小
        if (current_spawn_interval < 100) { // 例如，最小间隔100ms
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
        return nullptr; // 没有生成点则不生成僵尸
    }

    spawn_timer += delta;

    if (spawn_timer >= current_spawn_interval) { // 使用调整后的间隔
        spawn_timer = 0;

        std::uniform_real_distribution<> dis_chance(0.0, 1.0);
        if (dis_chance(rng) < spawn_chance) {
            // 随机决定生成哪种僵尸
            std::uniform_int_distribution<> type_dis(0, 2); // 0: Normal, 1: Cone, 2: Bucket
            int zombie_type = type_dis(rng);

            // 从墓碑列表中随机选择一个生成位置
            std::uniform_int_distribution<> pos_dis(0, spawn_positions.size() - 1);
            POINT base_spawn_pos = spawn_positions[pos_dis(rng)];

            // 在选定墓碑位置附近随机偏移一点
            POINT spawn_pos = base_spawn_pos;
            std::uniform_int_distribution<> offset_dis(-10, 10); // 较小的偏移，因为是基于墓碑精确位置
            spawn_pos.y += offset_dis(rng);
            // 可以选择也对x进行偏移
            // spawn_pos.x += offset_dis(rng);


            switch (zombie_type) {
            case 0:
                return new NormalZombie(spawn_pos);
            case 1:
                return new ConeZombie(spawn_pos);
            case 2:
                return new BucketZombie(spawn_pos);
            default:
                return new NormalZombie(spawn_pos); // 默认情况
            }
        }
    }
    return nullptr;
}