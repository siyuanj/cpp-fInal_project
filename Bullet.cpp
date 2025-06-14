#include "Bullet.h"
#include "Zombie.h" // 需要 Zombie 类的完整定义
#include "Animation.h" // 如果使用 Animation 类
#include <cmath>      // For std::sqrt
#include <limits>     // For std::numeric_limits
// 初始化静态成员变量
Atlas* NormalBullet::bullet_atlas = nullptr;


Bullet::Bullet(POINT pos, POINT target_pos, double init_speed,int damage)
    : anim(nullptr), position(pos), target_position(target_pos), // 存储目标位置
    speed(init_speed), damage_value(damage), is_active(true) {
    
}

Bullet::~Bullet() {
    if (anim) delete anim;
}


void Bullet::Update(int delta) {
    if (!is_active) return;

    // 子弹移动逻辑
    double dx_to_target = static_cast<double>(target_position.x) - position.x;
    double dy_to_target = static_cast<double>(target_position.y) - position.y;
    double distance_to_target = std::sqrt(dx_to_target * dx_to_target + dy_to_target * dy_to_target);

    double move_distance_this_frame = speed * (static_cast<double>(delta) / 50.0);

    if (distance_to_target <= move_distance_this_frame || distance_to_target == 0) {
        // 到达或超过目标点 (或者已经在目标点)
        position = target_position; // 直接移动到目标点
        is_active = false;          // 子弹到达目标后失效
    }
    else {
        // 向目标点移动
        position.x += static_cast<long>((dx_to_target / distance_to_target) * move_distance_this_frame);
        position.y += static_cast<long>((dy_to_target / distance_to_target) * move_distance_this_frame);
    }

    // 检查子弹是否飞出屏幕边界
     if (position.x > 1280 || position.x < 0 || position.y > 720 || position.y < 0) {
         is_active = false;
     }
}

void Bullet::Draw() {
    if (is_active && anim) {
        // 若后续添加动画则可通过draw进行调用
        anim->showimage(position.x, position.y, 1000 / 60);
    }
}

POINT Bullet::GetPosition() const { return position; }
bool Bullet::IsActive() const { return is_active; }
void Bullet::Deactivate() { is_active = false; }
int Bullet::GetDamage() const { return damage_value; }


NormalBullet::NormalBullet(POINT pos, POINT target_pos, double init_speed, int damage)
    : Bullet(pos, target_pos, init_speed, damage)
 {
    if (!bullet_atlas) {  
        bullet_atlas = new Atlas(_T("img/normal_bullet.gif"));
        //loadimage(bullet_img, _T("img/normal_bullet.g  if"));
    }
}

void NormalBullet::Draw() {
    if (IsActive() && bullet_atlas) {
        putimage_alpha(position.x, position.y, bullet_atlas->frame_list[0]);
		// 使用Atlas中的帧进行绘制
    }
    else if (is_active && anim) { // 如果后续需要使用 anim
        Bullet::Draw(); // 调用基类绘制（如果 anim 有效）
    }
}

// NormalBullet 清理静态资源
void NormalBullet::Cleanup() {
    if (bullet_atlas) {
        delete bullet_atlas;
        bullet_atlas = nullptr;
    }
}