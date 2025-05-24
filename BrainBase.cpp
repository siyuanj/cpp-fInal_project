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
    if (hp < 0) hp = 0;
}

bool BrainBase::IsAlive() const { return is_alive; }
bool BrainBase::IsPlaced() const { return is_placed; }

void BrainBase::Draw() {
    if (is_placed) {
        putimage_alpha(position.x - 50, position.y, brain_base->frame_list[0]);
    }
}