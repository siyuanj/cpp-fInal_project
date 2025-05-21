#include "Zombie.h"
#include <cmath>

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