#include "Zombie.h"
#include <cmath>

// 基础僵尸类实现
Zombie::Zombie(int init_hp, int init_attack_power, POINT init_pos, double init_speed,
               Atlas* atlas, int frame_interval, int atk_interval)
    : hp(init_hp), attack_power(init_attack_power), position(init_pos),
      speed(init_speed), is_alive(true), attack_interval(atk_interval), attack_timer(0) {
    anim = new Animation(atlas, frame_interval);
    target_position = { 0, init_pos.y };
}

Zombie::~Zombie() {
    if (anim) {
        delete anim;
    }
}

void Zombie::Update(int delta) {
    if (!is_alive) return;

    double dx = target_position.x - position.x;
    double dy = target_position.y - position.y;
    double distance = sqrt(dx * dx + dy * dy);

    if (distance > 1.0) {
        double normalized_dx = (dx / distance) * speed * delta / 1000.0;
        double normalized_dy = (dy / distance) * speed * delta / 1000.0;

        position.x += static_cast<long>(normalized_dx);
        position.y += static_cast<long>(normalized_dy);
    }

    attack_timer += delta;
    if (attack_timer >= attack_interval) {
        attack_timer = 0;
    }
}

void Zombie::Draw() {
    if (is_alive && anim) {
        anim->showimage(position.x, position.y, 1000 / 144);
    }
}

void Zombie::TakeDamage(int damage) {
    if (!is_alive) return;
    hp -= damage;
    if (hp <= 0) {
        hp = 0;
        is_alive = false;
    }
}

bool Zombie::IsAlive() const { return is_alive; }
POINT Zombie::GetPosition() const { return position; }
int Zombie::GetAttackPower() const { return attack_power; }

// 普通僵尸实现
NormalZombie::NormalZombie(POINT init_pos)
    : Zombie(100,                          // 生命值100
            10,                            // 攻击力10
            init_pos,                      // 初始位置
            50.0,                          // 移动速度50
            new Atlas(_T("img/normal_zombie_%d.png"), 22),  // 动画图集
            100,                           // 动画间隔100ms
            1000) {                        // 攻击间隔1000ms
}

// 有防具僵尸基类实现
ArmoredZombie::ArmoredZombie(int init_hp, int armor_init_hp, POINT init_pos, double init_speed,
                             Atlas* zombie_atlas, Atlas* armor_atlas,
                             int frame_interval, int atk_interval)
    : Zombie(init_hp, 10, init_pos, init_speed, zombie_atlas, frame_interval, atk_interval),
      armor_hp(armor_init_hp), has_armor(true) {
    this->armor_atlas = armor_atlas;
    armor_anim = new Animation(armor_atlas, frame_interval);
}

ArmoredZombie::~ArmoredZombie() {
    if (armor_anim) {
        delete armor_anim;
    }
    if (armor_atlas) {
        delete armor_atlas;
    }
}

void ArmoredZombie::TakeDamage(int damage) {
    if (!is_alive) return;

    if (has_armor) {
        // 防具优先受到伤害
        armor_hp -= damage;
        if (armor_hp <= 0) {
            has_armor = false;  // 防具被破坏
        }
    } else {
        // 没有防具时，直接伤害僵尸本体
        Zombie::TakeDamage(damage);
    }
}

void ArmoredZombie::Draw() {
    if (!is_alive) return;

    if (has_armor && armor_anim) {
        // 有防具时显示带防具的动画
        armor_anim->showimage(position.x, position.y, 1000 / 144);
    } else {
        // 没有防具时显示普通僵尸动画
        Zombie::Draw();
    }
}

// 路障僵尸实现
ConeZombie::ConeZombie(POINT init_pos)
    : ArmoredZombie(100,                   // 基础生命值100
                    80,                     // 路障防具血量80
                    init_pos,               // 初始位置
                    45.0,                   // 移动速度45
                    new Atlas(_T("img/normal_zombie_%d.png"), 22),    // 普通僵尸动画
                    new Atlas(_T("img/cone_head_zombie_%d.png"), 20),      // 路障僵尸动画
                    100,                    // 动画间隔100ms
                    1000) {                 // 攻击间隔1000ms
}

// 铁桶僵尸实现
BucketZombie::BucketZombie(POINT init_pos)
    : ArmoredZombie(100,                   // 基础生命值100
                    150,                    // 铁桶防具血量150
                    init_pos,               // 初始位置
                    40.0,                   // 移动速度40
                    new Atlas(_T("img/normal_zombie_%d.png"), 22),    // 普通僵尸动画
                    new Atlas(_T("img/bucket_head_zombie_%d.png"), 14),    // 铁桶僵尸动画
                    100,                    // 动画间隔100ms
                    1000) {                 // 攻击间隔1000ms
}