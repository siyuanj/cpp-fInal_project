#ifndef SPECIFIC_PLANTS_H
#define SPECIFIC_PLANTS_H

#include "Plant.h"     // 包含植物基类头文件
#include "Sun.h"       // 包含阳光类头文件 (向日葵用)
#include "Bullet.h"    // 包含子弹类头文件 (豌豆射手用)
#include <vector>      // 用于 Attack 方法的参数

class Zombie; // 前向声明"

class Sunflower : public ResourcePlant {
private:
    static Atlas* sun_atlas;
    std::vector<Sun*> suns;

public:
    Sunflower(POINT pos);
    ~Sunflower();
    void GenerateResource() override;
    void Update(int delta) override;
    void Draw() override;
};

class Peashooter : public AttackPlant {
public:
    Peashooter(POINT pos);
        // 实现具体的攻击行为：发射子弹

    void Attack(Zombie* target_zombie, std::vector<Bullet*>& bullets); // 修改参数
    // Update 方法已在 AttackPlant 基类中声明和部分实现 (计时器逻辑)
	// 此处实现Attack逻辑


};

class Repeater : public AttackPlant {
public:
    Repeater(POINT pos);
    void Attack(Zombie* target_zombie, std::vector<Bullet*>& bullets); // 发射两颗子弹
};

class WallNut : public DefensePlant {
public:
    WallNut(POINT pos);
    // Update 和 TakeDamage 已在 DefensePlant 基类中声明/实现

};
class TallWallNut : public DefensePlant {
public:
	// 高墙坚果类，继承自 DefensePlant
    // 未实现
    TallWallNut(POINT pos);
    // Update 和 TakeDamage 已在 DefensePlant 基类中声明/实现

};

#endif // SPECIFIC_PLANTS_H