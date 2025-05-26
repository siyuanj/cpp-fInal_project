#include "Zombie.h"
#include "Plant.h" // 确保包含了 Plant.h 以使用 DefensePlant
#include <cmath>
#include <random>
#include <algorithm> // 需要包含 <algorithm> 以使用 std::sort (如果需要排序) 或其他算法

// 基础僵尸类实现
Zombie::Zombie(int init_hp, int init_attack_power, POINT init_pos, double init_speed,
    Atlas * atlas, int frame_interval, int atk_interval)
    : hp(init_hp), max_hp(init_hp), attack_power(init_attack_power), position(init_pos),
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
    /*if ((!target_plant || !target_plant->IsAlive()) && (!target_brain || !target_brain->IsAlive())) {
        // 确认目标就不修改

    }*/

    FindNearestTarget(plants, brain);// 随时调整目标

    // 计算移动距离
    double move_distance = speed * delta / 100.0;//限制于画图函数，没办法更慢
 
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
    if (hp > 0 && max_hp > 0) { // 仅当存活且max_hp有效时绘制
        const int health_bar_width = 60; // 血条总宽度 
        const int health_bar_height = 8;  // 血条高度
        const int health_bar_offset_y = 10; // 血条距离僵尸顶部的偏移量

        int zombie_image_width = 160; // 假设僵尸图像平均宽度为80，具体值应根据实际图像调整
        // 如果 Atlas 或 IMAGE 类提供了获取宽度的方法，可以使用它
        // 例如: zombie_image_width = anim->getanim_atlas()->get_frame_width(); (假设有此方法)


        // 血条背景位置
        // 将血条居中显示在僵尸上方
        int bar_bg_x = position.x + (zombie_image_width / 2) - (health_bar_width / 2);
        int bar_bg_y = position.y - health_bar_offset_y - health_bar_height;

        // 绘制血条背景 (例如，深灰色)
        setfillcolor(RGB(80, 80, 80));
        solidrectangle(bar_bg_x, bar_bg_y, bar_bg_x + health_bar_width, bar_bg_y + health_bar_height);

        // 计算当前血量百分比和血条颜色
        float health_percentage = static_cast<float>(hp) / max_hp;
        int current_health_width = static_cast<int>(health_bar_width * health_percentage);

        COLORREF health_color;
        if (health_percentage > 0.66f) {
            health_color = RGB(0, 255, 0); // 绿色
        }
        else if (health_percentage > 0.33f) {
            health_color = RGB(255, 255, 0); // 黄色
        }
        else {
            health_color = RGB(255, 0, 0); // 红色
        }

        // 绘制当前血量
        setfillcolor(health_color);
        solidrectangle(bar_bg_x, bar_bg_y, bar_bg_x + current_health_width, bar_bg_y + health_bar_height);

        // 绘制血条边框
        setlinecolor(BLACK);
        rectangle(bar_bg_x, bar_bg_y, bar_bg_x + health_bar_width, bar_bg_y + health_bar_height);
    }
}

// 普通僵尸实现
NormalZombie::NormalZombie(POINT init_pos)
    : Zombie(150,                // 生命值
        10,                  // 攻击力
        init_pos,
        3,              // 速度提高到100
        new Atlas(_T("img/normal_zombies_%d.png"), 22),
        100,
        1000) {
}

// 精英僵尸实现
EliteZombie::EliteZombie(POINT init_pos)
    : Zombie(450,                // 生命值 (例如: 普通僵尸的2.5倍)
        25,                  // 攻击力 (例如: 普通僵尸的2.5倍)
        init_pos,            // 初始位置
        3,                 // 移动速度 
        new Atlas(_T("img/elite_zombie_%d.png"), 23), // 图像资源路径和帧数
        100,                 // 动画帧间隔 (与普通僵尸一致)
        1000) {              // 攻击间隔 (与普通僵尸一致)
    // EliteZombie 特有的初始化代码可以放在这里 (如果需要)
}

// 有防具僵尸基类实现
ArmoredZombie::ArmoredZombie(int init_hp, int armor_init_hp, POINT init_pos, double init_speed,
    Atlas* zombie_atlas, Atlas* armor_atlas,
    int frame_interval, int atk_interval)
    : Zombie(init_hp, 10, init_pos, init_speed, zombie_atlas, frame_interval, atk_interval),
    armor_hp(armor_init_hp),
    max_armor_hp(armor_init_hp), // 初始化 max_armor_hp
    has_armor(true) {
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

    // 1. 绘制动画
    if (has_armor && armor_hp > 0 && armor_anim) {
        // 有有效护甲时显示带防具的动画
        armor_anim->showimage(position.x, position.y, 1000 / 144);
    }
    else if (anim) { // 没有有效护甲时，显示基础僵尸动画 (anim 是 Zombie 基类的成员)
        anim->showimage(position.x, position.y, 1000 / 144);
    }

    // --- 通用绘制参数 ---
    const int bar_total_width = 50;     // 血条和护甲条的总宽度
    const int bar_height = 6;           // 每个条的高度
    const int bar_offset_y_base = 12;   // 基础Y轴偏移（最下面条的底部到僵尸顶部的距离）
    const int bar_spacing = 2;          // 条之间的垂直间距

    // 居中显示条
    int zombie_image_width = 160;
    int base_x = position.x + (zombie_image_width / 2) - (bar_total_width / 2);

    // --- 2. 绘制生命值条 (HP) ---
    // 生命值条总是显示（如果僵尸存活且有最大生命值）
    if (max_hp > 0) { // hp 可能为0但仍存活直到下一帧处理，所以基于max_hp判断是否应绘制
        int health_bar_y_top = position.y - bar_offset_y_base - bar_height;

        // 绘制背景
        setfillcolor(RGB(80, 80, 80)); // 深灰色背景
        solidrectangle(base_x, health_bar_y_top, base_x + bar_total_width, health_bar_y_top + bar_height);

        // 绘制前景 (当前血量)
        float health_percentage = (hp > 0) ? (static_cast<float>(hp) / max_hp) : 0.0f;
        int current_health_width = static_cast<int>(bar_total_width * health_percentage);

        COLORREF health_color;
        if (health_percentage > 0.66f) health_color = RGB(0, 255, 0);    // 绿色
        else if (health_percentage > 0.33f) health_color = RGB(255, 255, 0); // 黄色
        else health_color = RGB(255, 0, 0);    // 红色

        if (current_health_width > 0) {
            setfillcolor(health_color);
            solidrectangle(base_x, health_bar_y_top, base_x + current_health_width, health_bar_y_top + bar_height);
        }

        // 绘制边框
        setlinecolor(BLACK);
        rectangle(base_x, health_bar_y_top, base_x + bar_total_width, health_bar_y_top + bar_height);
    }

    // --- 3. 绘制护甲条 (如果僵尸有护甲且护甲值 > 0) ---
    if (has_armor && armor_hp > 0 && max_armor_hp > 0) {
        // 护甲条在生命值条的上方
        int armor_bar_y_top = position.y - bar_offset_y_base - bar_height - bar_spacing - bar_height;

        // 绘制背景
        setfillcolor(RGB(100, 100, 100)); // 略浅的灰色背景
        solidrectangle(base_x, armor_bar_y_top, base_x + bar_total_width, armor_bar_y_top + bar_height);

        // 绘制前景 (当前护甲值)
        float armor_percentage = static_cast<float>(armor_hp) / max_armor_hp;
        int current_armor_width = static_cast<int>(bar_total_width * armor_percentage);

        if (current_armor_width > 0) {
            setfillcolor(RGB(200, 200, 220)); // 浅蓝灰色或白色作为护甲颜色
            solidrectangle(base_x, armor_bar_y_top, base_x + current_armor_width, armor_bar_y_top + bar_height);
        }

        // 绘制边框
        setlinecolor(BLACK);
        rectangle(base_x, armor_bar_y_top, base_x + bar_total_width, armor_bar_y_top + bar_height);
    }
}

// 路障僵尸实现
ConeZombie::ConeZombie(POINT init_pos)
    : ArmoredZombie(200,
        80,
        init_pos,
        3.0,               // 速度提高到80
        new Atlas(_T("img/normal_zombies_%d.png"), 22),
        new Atlas(_T("img/cone_head_zombie_%d.png"), 20),
        100,
        1000) {
}

// 铁桶僵尸实现
BucketZombie::BucketZombie(POINT init_pos)
    : ArmoredZombie(200,
        150,
        init_pos,
        3.0,               // 速度提高到60
        new Atlas(_T("img/normal_zombies_%d.png"), 22),
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
            std::uniform_int_distribution<> type_dis(0, 3); // 0: Normal, 1: Cone, 2: Bucket, 3: Elite
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
            case 3: // 新增精英僵尸的生成
                return new EliteZombie(spawn_pos);
            default:
                return new NormalZombie(spawn_pos); // 默认情况
            }
        }
    }
    return nullptr;
}