#include "BrainBase.h"

BrainBase::BrainBase() : hp(1000), is_placed(false), is_alive(true) { // 初始HP可以调整
    brain_base = new Atlas(_T("img/brain_base.png")); // 假设Atlas构造函数只接受路径
    // 如果Atlas需要帧数，即使只有一帧，也需要传递 Atlas(_T("img/brain_base.png"), 1)
    // position 可以初始化为 {-1, -1} 或其他无效值
}

BrainBase::~BrainBase() {
    delete brain_base;
}

void BrainBase::SetPosition(POINT pos) {
    position = pos;
    is_placed = true;
}

POINT BrainBase::GetPosition() const { return position; }
int BrainBase::GetHP() const { return hp; }

void BrainBase::TakeDamage(int damage) {
    hp -= damage;
    if (hp < 0) {
        hp = 0;
        is_alive = false;
    }
}

bool BrainBase::IsAlive() const { return is_alive; }
bool BrainBase::IsPlaced() const { return is_placed; }

// 实现强化生命值的方法
void BrainBase::ApplyHealthUpgrade(int amount) {
    max_hp += amount; // 增加最大生命值
    hp += amount;     // 同时增加当前生命值
    // 可选：确保当前生命值不超过新的最大生命值（尽管在此逻辑下通常不会超过）
    if (hp > max_hp) {
        hp = max_hp;
    }
    // 确保生命值不会因为增加过多而溢出或变为负数（如果amount可能为负）
    if (hp < 0) hp = 0;
    if (max_hp < 0) max_hp = 0; // 虽然不太可能，但作为防御性编程
}

void BrainBase::Draw() {
    if (is_placed) {
        putimage_alpha(position.x - 50, position.y, brain_base->frame_list[0]);
    }
    // 绘制血条
    if (hp > 0 && max_hp > 0) { // 仅当存活且max_hp有效时绘制
        const int health_bar_width = 160; // 血条总宽度
        const int health_bar_height = 12;  // 血条高度
        const int health_bar_offset_y = -10; // 血条距离植物顶部的偏移量

        // 使用近似的植物图像宽度
        int plant_image_width = 250;


        // 血条背景位置
        int bar_bg_x = position.x + (plant_image_width / 2) - (health_bar_width / 2)-50;
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