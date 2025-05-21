#include "Plant.h"

Plant::Plant(int init_hp, POINT pos, int init_cost, Atlas* atlas, int frame_interval)
    : hp(init_hp), position(pos), cost(init_cost), is_alive(true) {
    anim = new Animation(atlas, frame_interval);
}

Plant::~Plant() {
    if (anim) delete anim;
}

int Plant::GetHP() const { return hp; }
POINT Plant::GetPosition() const { return position; }
int Plant::GetCost() const { return cost; }
bool Plant::IsAlive() const { return is_alive; }

void Plant::TakeDamage(int damage) {
    hp -= damage;
    if (hp <= 0) {
        hp = 0;
        is_alive = false;
    }
}

void Plant::Draw() {
    if (is_alive && anim) {
        anim->showimage(position.x, position.y, 1000 / 144);
    }
}

// AttackPlant Implementation
AttackPlant::AttackPlant(int init_hp, POINT pos, int init_cost, Atlas* atlas,
    int frame_interval, int power, int range, int interval)
    : Plant(init_hp, pos, init_cost, atlas, frame_interval),
    attack_power(power), attack_range(range),
    attack_interval(interval), attack_timer(0) {
}

void AttackPlant::Update(int delta) {
    attack_timer += delta;
    if (attack_timer >= attack_interval) {
        Attack();
        attack_timer = 0;
    }
}

// DefensePlant Implementation
DefensePlant::DefensePlant(int init_hp, POINT pos, int init_cost, Atlas* atlas,
    int frame_interval, int init_defense, bool priority)
    : Plant(init_hp, pos, init_cost, atlas, frame_interval),
    defense(init_defense), is_priority(priority) {
}

void DefensePlant::Update(int delta) {
    // Basic update logic for defense plants
}

void DefensePlant::TakeDamage(int damage) {
    int actual_damage = damage - defense;
    if (actual_damage > 0) {
        Plant::TakeDamage(actual_damage);
    }
}

bool DefensePlant::IsPriority() const { return is_priority; }

// ResourcePlant Implementation
ResourcePlant::ResourcePlant(int init_hp, POINT pos, int init_cost, Atlas* atlas,
    int frame_interval, int rate, int amount)
    : Plant(init_hp, pos, init_cost, atlas, frame_interval),
    resource_rate(rate), resource_amount(amount), resource_timer(0) {
}

void ResourcePlant::Update(int delta) {
    resource_timer += delta;
    if (resource_timer >= resource_rate) {
        GenerateResource();
        resource_timer = 0;
    }
}