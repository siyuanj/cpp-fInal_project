#include "SpecificPlants.h"

// Initialize static member
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
    ResourcePlant::Update(delta);

    for (auto sun : suns) {
        sun->Update(delta);
    }

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
        100, 20, 400, 1500) {
}

void Peashooter::Attack() {
    // TODO: Implement pea shooting logic
}

WallNut::WallNut(POINT pos)
    : DefensePlant(400, pos, 50, new Atlas(_T("img/wallnut_%d.png"), 15),
        150, 20, true) {
}
//因为格挡比攻击还高，所以杀不死