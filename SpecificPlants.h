#ifndef SPECIFIC_PLANTS_H
#define SPECIFIC_PLANTS_H

#include "Plant.h"
#include "Sun.h"

class Sunflower : public ResourcePlant {
private:
    static Atlas* sun_atlas;
    std::vector<Sun*> suns;

public:
    Sunflower(POINT pos);
    ~Sunflower();
    void GenerateResource() override;
    void Update(int delta) override;
    void Draw() override;
};

class Peashooter : public AttackPlant {
public:
    Peashooter(POINT pos);
    void Attack() override;
};

class WallNut : public DefensePlant {
public:
    WallNut(POINT pos);
};

#endif // SPECIFIC_PLANTS_H