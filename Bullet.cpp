#include "Bullet.h"
#include "Zombie.h" // ��Ҫ Zombie �����������
#include "Animation.h" // ���ʹ�� Animation ��
#include <cmath>      // For std::sqrt
#include <limits>     // For std::numeric_limits
// ��ʼ����̬��Ա����
Atlas* NormalBullet::bullet_atlas = nullptr;


Bullet::Bullet(POINT pos, POINT target_pos, double init_speed,int damage)
    : anim(nullptr), position(pos), target_position(target_pos), // �洢Ŀ��λ��
    speed(init_speed), damage_value(damage), is_active(true) {
    
}

Bullet::~Bullet() {
    if (anim) delete anim;
}

//void Bullet::Update(int delta, const std::vector<Zombie*>& all_zombies) {
//
//    if (!is_active) return;
//
//    // �����߼����� all_zombies ���ҵ�����Ĵ�ʬ
//    Zombie* nearest_zombie = nullptr;
//    //double min_dist_sq = std::numeric_limits<double>::max();//������
//    double min_dist_sq = 1200;
//    for (Zombie* zombie_ptr : all_zombies) {
//        if (zombie_ptr && zombie_ptr->IsAlive()) {
//            POINT zombie_pos = zombie_ptr->GetPosition();
//            double dx_to_zombie = static_cast<double>(zombie_pos.x) - position.x;
//            double dy_to_zombie = static_cast<double>(zombie_pos.y) - position.y;
//            double dist_sq = dx_to_zombie * dx_to_zombie + dy_to_zombie * dy_to_zombie;
//
//            if (dist_sq < min_dist_sq) {
//                min_dist_sq = dist_sq;
//                nearest_zombie = zombie_ptr;
//            }
//        }
//    }
//
//    if (nearest_zombie) {
//        // �����ӵ���Ŀ���Ϊ�����ʬ��λ��
//        target_position = nearest_zombie->GetPosition();
//        // ��ѡ: ���� target_position ʹ����׼��ʬ����
//        target_position.x += 72; // ���� Zombie ������Щ����
//        target_position.y += 84;
//    }
//    // ���û���ҵ����Ľ�ʬ���ӵ���������������һ�� target_position (�� initial_target_pos ����һ֡�ҵ��Ľ�ʬλ��)
//
//    // �ӵ��ƶ��߼� (����֮ǰ Bullet.cpp �е��߼�����)
//    double dx_to_target = static_cast<double>(target_position.x) - position.x;
//    double dy_to_target = static_cast<double>(target_position.y) - position.y;
//    double distance_to_target = std::sqrt(dx_to_target * dx_to_target + dy_to_target * dy_to_target);
//
//    double move_distance_this_frame = speed * (static_cast<double>(delta) / 1000.0);
//
//    if (distance_to_target <= move_distance_this_frame || distance_to_target == 0) {
//        // ����򳬹�Ŀ��� (�����Ѿ���Ŀ���)
//        // ����׷���ӵ���ʵ�ʵġ����С�����ײ��⴦��
//        // �˴����ƶ���Ŀ��㣬���Ŀ���Ƕ�̬�ģ���һ֡�����¼���
//        if (distance_to_target > 0) { // �������0����Ȼǰ���Ѿ������ distance_to_target == 0
//            position.x += static_cast<long>((dx_to_target / distance_to_target) * move_distance_this_frame);
//            position.y += static_cast<long>((dy_to_target / distance_to_target) * move_distance_this_frame);
//        }
//        // ���ϣ���ӵ��ڵ��ﾲ̬Ŀ������ʧ���������������� is_active = false
//        // ������׷���ƶ�Ŀ�꣬ͨ������ײ�������ʹ��ʧЧ
//    }
//    else {
//        // ��Ŀ����ƶ�
//        position.x += static_cast<long>((dx_to_target / distance_to_target) * move_distance_this_frame);
//        position.y += static_cast<long>((dy_to_target / distance_to_target) * move_distance_this_frame);
//    }
//
//    // ����ӵ��Ƿ�ɳ���Ļ�߽�
//    if (position.x > WIDTH || position.x < 0 || position.y > HEIGHT || position.y < 0) {
//        is_active = false;
//    }
//}

void Bullet::Update(int delta) {
    if (!is_active) return;

    // �ӵ��ƶ��߼�
    double dx_to_target = static_cast<double>(target_position.x) - position.x;
    double dy_to_target = static_cast<double>(target_position.y) - position.y;
    double distance_to_target = std::sqrt(dx_to_target * dx_to_target + dy_to_target * dy_to_target);

    double move_distance_this_frame = speed * (static_cast<double>(delta) / 1000.0);

    if (distance_to_target <= move_distance_this_frame || distance_to_target == 0) {
        // ����򳬹�Ŀ��� (�����Ѿ���Ŀ���)
        position = target_position; // ֱ���ƶ���Ŀ���
        is_active = false;          // �ӵ�����Ŀ���ʧЧ
    }
    else {
        // ��Ŀ����ƶ�
        position.x += static_cast<long>((dx_to_target / distance_to_target) * move_distance_this_frame);
        position.y += static_cast<long>((dy_to_target / distance_to_target) * move_distance_this_frame);
    }

    // ����ӵ��Ƿ�ɳ���Ļ�߽�
     if (position.x > 1280 || position.x < 0 || position.y > 720 || position.y < 0) {
         is_active = false;
     }
}

void Bullet::Draw() {
    if (is_active && anim) {
        // ��������Ӷ������ͨ��draw���е���
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
		// ʹ��Atlas�е�֡���л���
    }
    else if (is_active && anim) { // ���������Ҫʹ�� anim
        Bullet::Draw(); // ���û�����ƣ���� anim ��Ч��
    }
}

// NormalBullet ����̬��Դ
void NormalBullet::Cleanup() {
    if (bullet_atlas) {
        delete bullet_atlas;
        bullet_atlas = nullptr;
    }
}