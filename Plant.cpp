#include "Plant.h"
#include "Zombie.h"

Plant::Plant(int init_hp, POINT pos, int init_cost, Atlas* atlas, int frame_interval)
    : hp(init_hp), max_hp(init_hp), position(pos), cost(init_cost), is_alive(true) {
    
    anim = new Animation(atlas, frame_interval);
	// 此处需要保证传入了非空的Atlas指针
}

Plant::~Plant() {
    if (anim) delete anim;
}

int Plant::GetHP() const { return hp; }
int Plant::GetMaxHP() const { return max_hp; } // 获取最大生命值的方法
POINT Plant::GetPosition() const { return position; }
int Plant::GetCost() const { return cost; }
bool Plant::IsAlive() const { return is_alive; }

void Plant::TakeDamage(int damage) {
    hp -= damage;
    if (hp <= 0) {
        hp = 0;
        is_alive = false;
    }
}

void Plant::Draw() {
    if (is_alive && anim) {
        anim->showimage(position.x, position.y, 1000 / 144);

        // 绘制血条
        if (hp > 0 && max_hp > 0) { // 仅当存活且max_hp有效时绘制
            const int health_bar_width = 40; // 血条总宽度
            const int health_bar_height = 6;  // 血条高度
            const int health_bar_offset_y = 8; // 血条距离植物顶部的偏移量

            // 使用近似的植物图像宽度
            int plant_image_width = 70;


            // 血条背景位置
            int bar_bg_x = position.x + (plant_image_width / 2) - (health_bar_width / 2);
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

            setlinecolor(BLACK);// 血条边框
            rectangle(bar_bg_x, bar_bg_y, bar_bg_x + health_bar_width, bar_bg_y + health_bar_height);
        }
    }
}

// AttackPlant Implementation
AttackPlant::AttackPlant(int init_hp, POINT pos, int init_cost, Atlas* atlas,
    int frame_interval, int power, int range, int interval)
    : Plant(init_hp, pos, init_cost, atlas, frame_interval),
    attack_power(power), attack_range(range),
    attack_interval(interval), attack_timer(0) {
}
void AttackPlant::Update(int delta) {
    if (!is_alive) return; // 如果植物死亡，则不进行更新

    //// 调用基类 Plant 的 Update 方法来处理通用逻辑（例如动画更新）
    //Plant::Update(delta);

    //// AttackPlant 特有的非攻击逻辑更新可以在这里添加，
    ////对于 AttackPlant，这个 Update(int) 可能只需要调用基类的 Update。
}

void AttackPlant::UpdateAttackLogic(int delta, std::vector<Zombie*>& zombies, std::vector<Bullet*>& bullets) {
    if (!is_alive) return;
    attack_timer += delta;

    if (attack_timer >= attack_interval) {
        Zombie* target_zombie = nullptr;
        double min_dist_sq = attack_range * attack_range; // 使用距离的平方以避免开方运算

        for (Zombie* zombie : zombies) {
            if (zombie && zombie->IsAlive()) {
                POINT zombie_pos = zombie->GetPosition();
                double dist_x = static_cast<double>(position.x) - zombie_pos.x;
                double dist_y = static_cast<double>(position.y) - zombie_pos.y;
                double current_dist_sq = dist_x * dist_x + dist_y * dist_y;

                if (current_dist_sq < min_dist_sq) {
                    min_dist_sq = current_dist_sq;
                    target_zombie = zombie;
                }
            }
        }// 确定最近的僵尸

        if (target_zombie) {
            // 将目标僵尸传递给 Attack 方法
            Attack(target_zombie, bullets);
            attack_timer = 0;     // 重置攻击计时器
        }
        // 如果没有目标，则不攻击，计时器会继续累加或按需重置
    }
}

// DefensePlant Implementation
DefensePlant::DefensePlant(int init_hp, POINT pos, int init_cost, Atlas* atlas,
    int frame_interval, int init_defense, bool priority)
    : Plant(init_hp, pos, init_cost, atlas, frame_interval),
    defense(init_defense), is_priority(priority) {
}

void DefensePlant::Update(int delta) {
    if (!is_alive) return;
    // 防御植物通常没有主动的攻击行为，其主要逻辑在 TakeDamage 中
}

void DefensePlant::TakeDamage(int damage) {
    int actual_damage = damage - defense;
    if (actual_damage > 0) {
        Plant::TakeDamage(actual_damage);
    }
    else Plant::TakeDamage(1);// 强制扣1滴血
}

bool DefensePlant::IsPriority() const { return is_priority; }

// ResourcePlant Implementation
ResourcePlant::ResourcePlant(int init_hp, POINT pos, int init_cost, Atlas* atlas,
    int frame_interval, int rate, int amount)
    : Plant(init_hp, pos, init_cost, atlas, frame_interval),
    resource_rate(rate), resource_amount(amount), resource_timer(0) {
}

void ResourcePlant::Update(int delta) {
    if (!is_alive) return;
    resource_timer += delta;
    if (resource_timer >= resource_rate) {
        GenerateResource(); // 调用派生类实现的具体资源生成方法
        resource_timer = 0; // 重置计时器
    }
}