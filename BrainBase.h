#ifndef BRAIN_BASE_H
#define BRAIN_BASE_H

#include <graphics.h>
#include "Animation.h"  // ���Animation.h�İ�����ʹ��putimage_alpha����

class BrainBase {
private:
    int hp;
    POINT position;
    IMAGE* img;
    bool is_placed;

public:
    BrainBase();
    ~BrainBase();
    void SetPosition(POINT pos);
    POINT GetPosition() const;
    int GetHP() const;
    void TakeDamage(int damage);
    bool IsAlive() const;
    bool IsPlaced() const;
    void Draw();
};

#endif // BRAIN_BASE_H