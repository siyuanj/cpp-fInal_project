#ifndef BRAIN_BASE_H
#define BRAIN_BASE_H

#include <graphics.h>
#include "Animation.h"  // 添加Animation.h的包含以使用putimage_alpha函数

class BrainBase {
private:
    int hp = 1000;
	int max_hp = 1000;
    POINT position;
    Atlas* brain_base;
    bool is_placed;
	bool is_alive;

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
    void ApplyHealthUpgrade(int amount); // 声明强化生命值的方法
    void Reset(); // 新增：重置基地状态的方法

};

#endif // BRAIN_BASE_H