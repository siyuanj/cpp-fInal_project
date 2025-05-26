#ifndef SPECIFIC_PLANTS_H
#define SPECIFIC_PLANTS_H

#include "Plant.h"     // ����ֲ�����ͷ�ļ�
#include "Sun.h"       // ����������ͷ�ļ� (���տ���)
#include "Bullet.h"    // �����ӵ���ͷ�ļ� (�㶹������)
#include <vector>      // ���� Attack �����Ĳ���

class Zombie; // ǰ������"

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
        // ʵ�־���Ĺ�����Ϊ�������ӵ�

    void Attack(Zombie* target_zombie, std::vector<Bullet*>& bullets); // �޸Ĳ���
    // Update �������� AttackPlant �����������Ͳ���ʵ�� (��ʱ���߼�)
	// �˴�ʵ��Attack�߼�


};

class Repeater : public AttackPlant {
public:
    Repeater(POINT pos);
    void Attack(Zombie* target_zombie, std::vector<Bullet*>& bullets); // ���������ӵ�
};

class WallNut : public DefensePlant {
public:
    WallNut(POINT pos);
    // Update �� TakeDamage ���� DefensePlant ����������/ʵ��

};
class TallWallNut : public DefensePlant {
public:
	// ��ǽ����࣬�̳��� DefensePlant
    // δʵ��
    TallWallNut(POINT pos);
    // Update �� TakeDamage ���� DefensePlant ����������/ʵ��

};

#endif // SPECIFIC_PLANTS_H