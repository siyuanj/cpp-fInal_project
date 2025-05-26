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