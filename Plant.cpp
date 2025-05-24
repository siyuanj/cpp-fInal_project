#include "Plant.h"

Plant::Plant(int init_hp, POINT pos, int init_cost, Atlas* atlas, int frame_interval)
    : hp(init_hp), max_hp(init_hp), position(pos), cost(init_cost), is_alive(true) {
    anim = new Animation(atlas, frame_interval);
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

            setlinecolor(BLACK);
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
    attack_timer += delta;
    if (attack_timer >= attack_interval) {
        Attack();
        attack_timer = 0;
    }
}

// DefensePlant Implementation
DefensePlant::DefensePlant(int init_hp, POINT pos, int init_cost, Atlas* atlas,
    int frame_interval, int init_defense, bool priority)
    : Plant(init_hp, pos, init_cost, atlas, frame_interval),
    defense(init_defense), is_priority(priority) {
}

void DefensePlant::Update(int delta) {
    // Basic update logic for defense plants
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
    resource_timer += delta;
    if (resource_timer >= resource_rate) {
        GenerateResource();
        resource_timer = 0;
    }
}