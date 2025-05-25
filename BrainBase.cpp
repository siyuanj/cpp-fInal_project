#include "BrainBase.h"

BrainBase::BrainBase() : hp(1000), is_placed(false), is_alive(true) { // ��ʼHP���Ե���
    brain_base = new Atlas(_T("img/brain_base.png")); // ����Atlas���캯��ֻ����·��
    // ���Atlas��Ҫ֡������ʹֻ��һ֡��Ҳ��Ҫ���� Atlas(_T("img/brain_base.png"), 1)
    // position ���Գ�ʼ��Ϊ {-1, -1} ��������Чֵ
}

BrainBase::~BrainBase() {
    delete brain_base;
}

void BrainBase::SetPosition(POINT pos) {
    position = pos;
    is_placed = true;
}

POINT BrainBase::GetPosition() const { return position; }
int BrainBase::GetHP() const { return hp; }

void BrainBase::TakeDamage(int damage) {
    hp -= damage;
    if (hp < 0) {
        hp = 0;
        is_alive = false;
    }
}

bool BrainBase::IsAlive() const { return is_alive; }
bool BrainBase::IsPlaced() const { return is_placed; }

void BrainBase::Draw() {
    if (is_placed) {
        putimage_alpha(position.x - 50, position.y, brain_base->frame_list[0]);
    }
    // ����Ѫ��
    if (hp > 0 && max_hp > 0) { // ���������max_hp��Чʱ����
        const int health_bar_width = 160; // Ѫ���ܿ��
        const int health_bar_height = 12;  // Ѫ���߶�
        const int health_bar_offset_y = -10; // Ѫ������ֲ�ﶥ����ƫ����

        // ʹ�ý��Ƶ�ֲ��ͼ����
        int plant_image_width = 250;


        // Ѫ������λ��
        int bar_bg_x = position.x + (plant_image_width / 2) - (health_bar_width / 2)-50;
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

        // ��ѡ������Ѫ���߿�
        setlinecolor(BLACK);
        rectangle(bar_bg_x, bar_bg_y, bar_bg_x + health_bar_width, bar_bg_y + health_bar_height);
    }
}