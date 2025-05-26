#include "SpecificPlants.h"
#include "Zombie.h" // 需要 Zombie 类的定义来进行索敌
#include "Animation.h"  

// Sunflower 静态成员初始化
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
    // 调用基类的 Update 方法  
    ResourcePlant::Update(delta);  

    // 更新所有的阳光对象  
    for (auto sun : suns) {  
        sun->Update(delta);  
    }  

    // 移除不活跃的阳光对象  
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
    // attack_power = 20 (伤害)
    // attack_range = 500 (攻击距离，像素)
    // attack_interval = 1500 (攻击间隔，毫秒)
}

void Peashooter::Attack(Zombie* target_zombie, std::vector<Bullet*>& bullets) {
    if (!target_zombie) { // 如果没有目标，则不攻击
        return;
    }

    // 1. 确定子弹发射位置
    POINT bullet_start_pos = position;
    bullet_start_pos.x += 39;
    bullet_start_pos.y += 3;

    // 2. 创建一颗新的子弹，并设置其追踪目标
    double bullet_speed = 100.0;
    // 假设你的 Bullet 构造函数或一个设置目标的方法可以接收 Zombie*
    // 你可能需要修改 Bullet 或 NormalBullet 的构造函数来接收 Zombie* target
    // 或者添加一个 SetTarget(Zombie* target) 方法
    NormalBullet* new_bullet = new NormalBullet(bullet_start_pos, target_zombie->GetPosition(), bullet_speed, attack_power);
    // new_bullet->SetTarget(target_zombie); // 如果你的 Bullet 类有这样的方法

    bullets.push_back(new_bullet);
    // 音效...
}

Repeater::Repeater(POINT pos)
    : AttackPlant(100, pos, 200, new Atlas(_T("img/repeater_%d.png"), 15),
        100, 20, 500, 800) {
    // 参数说明 (基于Peashooter的AttackPlant构造函数推断):
    // HP: 100
    // 位置: pos
    // 阳光花费: 200 (Repeater通常比Peashooter贵)
    // 图像: new Atlas(_T("img/repeater_%d.png"), 15) (15帧图像)
    // frame_interval: 100 (与Peashooter保持一致)
    // 攻击力 (每颗子弹): 20
    // 攻击范围: 500
    // 攻击间隔 (发射一对子弹的间隔): 800ms 
    // 你可以根据游戏平衡调整这些数值
}

void Repeater::Attack(Zombie* target_zombie, std::vector<Bullet*>& bullets) {
    if (!target_zombie) { // 如果没有目标，则不攻击
        return;
    }

    double bullet_speed = 100.0; // 子弹速度，与Peashooter一致

    // 第一颗子弹
    POINT bullet_start_pos1 = position;
    bullet_start_pos1.x += 39; // 主要发射点，可以根据Repeater的图像调整
    bullet_start_pos1.y += 3;  // Y轴偏移，与Peashooter一致
    NormalBullet* new_bullet1 = new NormalBullet(bullet_start_pos1, target_zombie->GetPosition(), bullet_speed, attack_power);
    bullets.push_back(new_bullet1);

    // 第二颗子弹
    // 为了体现"连续"，第二颗子弹可以从略微不同的位置发射，或者在同一位置紧接着创建
    // 从一个稍微靠前（或旁边）的位置发射，以示区别
    POINT bullet_start_pos2 = position;
    bullet_start_pos2.x += 25; // 第二个发射点X偏移 (例如，比第一颗子弹靠左14像素: 39-14=25)
    bullet_start_pos2.y += 3;  // Y轴偏移，与Peashooter一致
    NormalBullet* new_bullet2 = new NormalBullet(bullet_start_pos2, target_zombie->GetPosition(), bullet_speed, attack_power);
    bullets.push_back(new_bullet2);

    // 可在此处添加发射音效等
}


WallNut::WallNut(POINT pos)
    : DefensePlant(400, pos, 50, new Atlas(_T("img/wallnut_%d.png"), 15),
        150, 20, true) {
}
// 属性分别为： 生命值(400), 位置(pos), 宽度(50), 图像(new Atlas("img/wallnut_%d.png", 15)),
// 攻击间隔(150), 攻击力(20), 是否可以格挡(true)
//因为格挡比攻击还高，所以杀不死

TallWallNut :: TallWallNut(POINT pos)
    : DefensePlant(1000, pos, 50, new Atlas(_T("img/TallWallNut_%d.png"), 14),
        150, 20, true) {
}