#include "Plant.h"
#include "Zombie.h"

Plant::Plant(int init_hp, POINT pos, int init_cost, Atlas* atlas, int frame_interval)
    : hp(init_hp), max_hp(init_hp), position(pos), cost(init_cost), is_alive(true) {
    
    anim = new Animation(atlas, frame_interval);
	// �˴���Ҫ��֤�����˷ǿյ�Atlasָ��
}

Plant::~Plant() {
    if (anim) delete anim;
}

int Plant::GetHP() const { return hp; }
int Plant::GetMaxHP() const { return max_hp; } // ��ȡ�������ֵ�ķ���
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

        // ����Ѫ��
        if (hp > 0 && max_hp > 0) { // ���������max_hp��Чʱ����
            const int health_bar_width = 40; // Ѫ���ܿ��
            const int health_bar_height = 6;  // Ѫ���߶�
            const int health_bar_offset_y = 8; // Ѫ������ֲ�ﶥ����ƫ����

            // ʹ�ý��Ƶ�ֲ��ͼ����
            int plant_image_width = 70;


            // Ѫ������λ��
            int bar_bg_x = position.x + (plant_image_width / 2) - (health_bar_width / 2);
            int bar_bg_y = position.y - health_bar_offset_y - health_bar_height;

            // ����Ѫ������ (���磬���ɫ)
            setfillcolor(RGB(80, 80, 80));
            solidrectangle(bar_bg_x, bar_bg_y, bar_bg_x + health_bar_width, bar_bg_y + health_bar_height);

            // ���㵱ǰѪ���ٷֱȺ�Ѫ����ɫ
            float health_percentage = static_cast<float>(hp) / max_hp;
            int current_health_width = static_cast<int>(health_bar_width * health_percentage);

            COLORREF health_color;
            if (health_percentage > 0.66f) {
                health_color = RGB(0, 255, 0); // ��ɫ
            }
            else if (health_percentage > 0.33f) {
                health_color = RGB(255, 255, 0); // ��ɫ
            }
            else {
                health_color = RGB(255, 0, 0); // ��ɫ
            }

            // ���Ƶ�ǰѪ��
            setfillcolor(health_color);
            solidrectangle(bar_bg_x, bar_bg_y, bar_bg_x + current_health_width, bar_bg_y + health_bar_height);

            setlinecolor(BLACK);// Ѫ���߿�
            rectangle(bar_bg_x, bar_bg_y, bar_bg_x + health_bar_width, bar_bg_y + health_bar_height);
        }
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
    if (!is_alive) return; // ���ֲ���������򲻽��и���

    //// ���û��� Plant �� Update ����������ͨ���߼������綯�����£�
    //Plant::Update(delta);

    //// AttackPlant ���еķǹ����߼����¿�����������ӣ�
    ////���� AttackPlant����� Update(int) ����ֻ��Ҫ���û���� Update��
}

void AttackPlant::UpdateAttackLogic(int delta, std::vector<Zombie*>& zombies, std::vector<Bullet*>& bullets) {
    if (!is_alive) return;
    attack_timer += delta;

    if (attack_timer >= attack_interval) {
        Zombie* target_zombie = nullptr;
        double min_dist_sq = attack_range * attack_range; // ʹ�þ����ƽ���Ա��⿪������

        for (Zombie* zombie : zombies) {
            if (zombie && zombie->IsAlive()) {
                POINT zombie_pos = zombie->GetPosition();
                double dist_x = static_cast<double>(position.x) - zombie_pos.x;
                double dist_y = static_cast<double>(position.y) - zombie_pos.y;
                double current_dist_sq = dist_x * dist_x + dist_y * dist_y;

                if (current_dist_sq < min_dist_sq) {
                    min_dist_sq = current_dist_sq;
                    target_zombie = zombie;
                }
            }
        }// ȷ������Ľ�ʬ

        if (target_zombie) {
            // ��Ŀ�꽩ʬ���ݸ� Attack ����
            Attack(target_zombie, bullets);
            attack_timer = 0;     // ���ù�����ʱ��
        }
        // ���û��Ŀ�꣬�򲻹�������ʱ��������ۼӻ�������
    }
}

// DefensePlant Implementation
DefensePlant::DefensePlant(int init_hp, POINT pos, int init_cost, Atlas* atlas,
    int frame_interval, int init_defense, bool priority)
    : Plant(init_hp, pos, init_cost, atlas, frame_interval),
    defense(init_defense), is_priority(priority) {
}

void DefensePlant::Update(int delta) {
    if (!is_alive) return;
    // ����ֲ��ͨ��û�������Ĺ�����Ϊ������Ҫ�߼��� TakeDamage ��
}

void DefensePlant::TakeDamage(int damage) {
    int actual_damage = damage - defense;
    if (actual_damage > 0) {
        Plant::TakeDamage(actual_damage);
    }
    else Plant::TakeDamage(1);// ǿ�ƿ�1��Ѫ
}

bool DefensePlant::IsPriority() const { return is_priority; }

// ResourcePlant Implementation
ResourcePlant::ResourcePlant(int init_hp, POINT pos, int init_cost, Atlas* atlas,
    int frame_interval, int rate, int amount)
    : Plant(init_hp, pos, init_cost, atlas, frame_interval),
    resource_rate(rate), resource_amount(amount), resource_timer(0) {
}

void ResourcePlant::Update(int delta) {
    if (!is_alive) return;
    resource_timer += delta;
    if (resource_timer >= resource_rate) {
        GenerateResource(); // ����������ʵ�ֵľ�����Դ���ɷ���
        resource_timer = 0; // ���ü�ʱ��
    }
}