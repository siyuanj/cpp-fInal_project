#include "SpecificPlants.h"
#include "Zombie.h" // ��Ҫ Zombie ��Ķ�������������
#include "Animation.h"  

// Sunflower ��̬��Ա��ʼ��
Atlas* Sunflower::sun_atlas = nullptr;

Sunflower::Sunflower(POINT pos)
    : ResourcePlant(100, pos, 50, new Atlas(_T("img/sunflower_%d.png"), 17), 100, 2000, 25) {
    if (!sun_atlas) {
        sun_atlas = new Atlas(_T("img/sun_%d.png"), 28);
    }
}

Sunflower::~Sunflower() {
    for (auto sun : suns) {
        delete sun;
    }
    suns.clear();
}

void Sunflower::GenerateResource() {
    POINT sun_pos = position;
    sun_pos.x += 40;
    sun_pos.y += 40;
    sun_pos.x += 20;
    suns.push_back(new Sun(sun_pos, sun_atlas));
}

void Sunflower::Update(int delta) {  
    // ���û���� Update ����  
    ResourcePlant::Update(delta);  

    // �������е��������  
    for (auto sun : suns) {  
        sun->Update(delta);  
    }  

    // �Ƴ�����Ծ���������  
    suns.erase(  
        std::remove_if(suns.begin(), suns.end(),  
            [](Sun* sun) {  
                if (!sun->IsActive()) {  
                    delete sun;  
                    return true;  
                }  
                return false;  
            }  
        ),  
        suns.end()  
    );  
}

void Sunflower::Draw() {
    Plant::Draw();
    for (auto sun : suns) {
        sun->Draw();
    }
}

Peashooter::Peashooter(POINT pos)
    : AttackPlant(100, pos, 100, new Atlas(_T("img/peashooter_%d.png"), 12),
        100, 20, 500, 800) {
    // attack_power = 20 (�˺�)
    // attack_range = 500 (�������룬����)
    // attack_interval = 1500 (�������������)
}

void Peashooter::Attack(Zombie* target_zombie, std::vector<Bullet*>& bullets) {
    if (!target_zombie) { // ���û��Ŀ�꣬�򲻹���
        return;
    }

    // 1. ȷ���ӵ�����λ��
    POINT bullet_start_pos = position;
    bullet_start_pos.x += 39;
    bullet_start_pos.y += 3;

    // 2. ����һ���µ��ӵ�����������׷��Ŀ��
    double bullet_speed = 100.0;
    // ������� Bullet ���캯����һ������Ŀ��ķ������Խ��� Zombie*
    // �������Ҫ�޸� Bullet �� NormalBullet �Ĺ��캯�������� Zombie* target
    // �������һ�� SetTarget(Zombie* target) ����
    NormalBullet* new_bullet = new NormalBullet(bullet_start_pos, target_zombie->GetPosition(), bullet_speed, attack_power);
    // new_bullet->SetTarget(target_zombie); // ������ Bullet ���������ķ���

    bullets.push_back(new_bullet);
    // ��Ч...
}

WallNut::WallNut(POINT pos)
    : DefensePlant(400, pos, 50, new Atlas(_T("img/wallnut_%d.png"), 15),
        150, 20, true) {
}
// ���Էֱ�Ϊ�� ����ֵ(400), λ��(pos), ���(50), ͼ��(new Atlas("img/wallnut_%d.png", 15)),
// �������(150), ������(20), �Ƿ���Ը�(true)
//��Ϊ�񵲱ȹ������ߣ�����ɱ����

TallWallNut :: TallWallNut(POINT pos)
    : DefensePlant(1000, pos, 50, new Atlas(_T("img/TallWallNut_%d.png"), 14),
        150, 20, true) {
}