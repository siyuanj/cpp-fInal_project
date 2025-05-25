#ifndef BULLET_H
#define BULLET_H
#include <cmath> // 用于量归一化 (如果需要)
#include <vector>

#include "Animation.h"

#define WIDTH 1280
#define HEIGHT 720 // 添加屏幕高度定

class Bullet {
protected:
    Animation* anim;      // 动画对象指针 (如果子弹需要播放动画则需使用)
    // 目前不需要使用
    double speed = 100;// 默认速度
    POINT position;
    POINT target_position; // 子弹的目标位置

    bool is_active;
    int damage_value;     // 子弹造成的伤害值

public:
    // 构造函数：初始化子弹的位置、初始速度和伤害值
    Bullet(POINT pos, POINT target_pos, double init_speed, int damage);
    // 虚析构函数：确保派生类的析构函数能被正确调用
    virtual ~Bullet();
    // 虚更新函数：每帧调用，用于更新子弹的状态（例如，位置）
    // delta为距离上一帧的时间间隔（毫秒）
    virtual void Update(int delta, const std::vector<Zombie*>& all_zombies);
    // 虚绘制函数：每帧调用，用于在屏幕上绘制子弹
    virtual void Draw();

    // 获取子弹当前位置
    POINT GetPosition() const;
    // 检查子弹是否处于活动状态
    bool IsActive() const;
    // 将子弹设置为非活动状态（例如，击中目标或飞出屏幕后）
    void Deactivate();
    // 获取子弹的伤害值
    int GetDamage() const;
};

class NormalBullet : public Bullet {
private:
    static Atlas* bullet_atlas; // 普通子弹的静态图集指针 (所有普通子弹实例共享)
public:
    NormalBullet(POINT pos, POINT target_pos, double init_speed, int damage);
    void Draw() override;
    static void Cleanup();
};

#endif // BULLET_H