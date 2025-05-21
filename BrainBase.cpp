#include "BrainBase.h"

BrainBase::BrainBase() : hp(500), is_placed(false) {
    img = new IMAGE();
    loadimage(img, _T("img/brain_base.png"));
}

BrainBase::~BrainBase() {
    delete img;
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

bool BrainBase::IsAlive() const { return hp > 0; }
bool BrainBase::IsPlaced() const { return is_placed; }

void BrainBase::Draw() {
    if (is_placed) {
        putimage_alpha(position.x - 50, position.y - 50, img);
    }
}