#ifndef BULLET_H
#define BULLET_H
#include <cmath> // ��������һ�� (�����Ҫ)
#include <vector>

#include "Animation.h"

#define WIDTH 1280
#define HEIGHT 720 // �����Ļ�߶ȶ�

class Bullet {
protected:
    Animation* anim;      // ��������ָ�� (����ӵ���Ҫ���Ŷ�������ʹ��)
    // Ŀǰ����Ҫʹ��
    double speed = 100;// Ĭ���ٶ�
    POINT position;
    POINT target_position; // �ӵ���Ŀ��λ��

    bool is_active;
    int damage_value;     // �ӵ���ɵ��˺�ֵ

public:
    // ���캯������ʼ���ӵ���λ�á���ʼ�ٶȺ��˺�ֵ
    Bullet(POINT pos, POINT target_pos, double init_speed, int damage);
    // ������������ȷ������������������ܱ���ȷ����
    virtual ~Bullet();
    // ����º�����ÿ֡���ã����ڸ����ӵ���״̬�����磬λ�ã�
    // deltaΪ������һ֡��ʱ���������룩
    virtual void Update(int delta, const std::vector<Zombie*>& all_zombies);
    // ����ƺ�����ÿ֡���ã���������Ļ�ϻ����ӵ�
    virtual void Draw();

    // ��ȡ�ӵ���ǰλ��
    POINT GetPosition() const;
    // ����ӵ��Ƿ��ڻ״̬
    bool IsActive() const;
    // ���ӵ�����Ϊ�ǻ״̬�����磬����Ŀ���ɳ���Ļ��
    void Deactivate();
    // ��ȡ�ӵ����˺�ֵ
    int GetDamage() const;
};

class NormalBullet : public Bullet {
private:
    static Atlas* bullet_atlas; // ��ͨ�ӵ��ľ�̬ͼ��ָ�� (������ͨ�ӵ�ʵ������)
public:
    NormalBullet(POINT pos, POINT target_pos, double init_speed, int damage);
    void Draw() override;
    static void Cleanup();
};

#endif // BULLET_H