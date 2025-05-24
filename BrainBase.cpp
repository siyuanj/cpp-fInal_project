#include "BrainBase.h"

BrainBase::BrainBase() : hp(1000), is_placed(false), is_alive(true) { // ��ʼHP���Ե���
    brain_base = new Atlas(_T("img/brain_base.png")); // ����Atlas���캯��ֻ����·��
    // ���Atlas��Ҫ֡������ʹֻ��һ֡��Ҳ��Ҫ���� Atlas(_T("img/brain_base.png"), 1)
    // position ���Գ�ʼ��Ϊ {-1, -1} ��������Чֵ
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