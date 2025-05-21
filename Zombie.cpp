#include "Zombie.h"
#include <cmath>

// ������ʬ��ʵ��
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

// ��ͨ��ʬʵ��
NormalZombie::NormalZombie(POINT init_pos)
    : Zombie(100,                          // ����ֵ100
            10,                            // ������10
            init_pos,                      // ��ʼλ��
            50.0,                          // �ƶ��ٶ�50
            new Atlas(_T("img/normal_zombie_%d.png"), 22),  // ����ͼ��
            100,                           // �������100ms
            1000) {                        // �������1000ms
}

// �з��߽�ʬ����ʵ��
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
        // ���������ܵ��˺�
        armor_hp -= damage;
        if (armor_hp <= 0) {
            has_armor = false;  // ���߱��ƻ�
        }
    } else {
        // û�з���ʱ��ֱ���˺���ʬ����
        Zombie::TakeDamage(damage);
    }
}

void ArmoredZombie::Draw() {
    if (!is_alive) return;

    if (has_armor && armor_anim) {
        // �з���ʱ��ʾ�����ߵĶ���
        armor_anim->showimage(position.x, position.y, 1000 / 144);
    } else {
        // û�з���ʱ��ʾ��ͨ��ʬ����
        Zombie::Draw();
    }
}

// ·�Ͻ�ʬʵ��
ConeZombie::ConeZombie(POINT init_pos)
    : ArmoredZombie(100,                   // ��������ֵ100
                    80,                     // ·�Ϸ���Ѫ��80
                    init_pos,               // ��ʼλ��
                    45.0,                   // �ƶ��ٶ�45
                    new Atlas(_T("img/normal_zombie_%d.png"), 22),    // ��ͨ��ʬ����
                    new Atlas(_T("img/cone_head_zombie_%d.png"), 20),      // ·�Ͻ�ʬ����
                    100,                    // �������100ms
                    1000) {                 // �������1000ms
}

// ��Ͱ��ʬʵ��
BucketZombie::BucketZombie(POINT init_pos)
    : ArmoredZombie(100,                   // ��������ֵ100
                    150,                    // ��Ͱ����Ѫ��150
                    init_pos,               // ��ʼλ��
                    40.0,                   // �ƶ��ٶ�40
                    new Atlas(_T("img/normal_zombie_%d.png"), 22),    // ��ͨ��ʬ����
                    new Atlas(_T("img/bucket_head_zombie_%d.png"), 14),    // ��Ͱ��ʬ����
                    100,                    // �������100ms
                    1000) {                 // �������1000ms
}