#include<graphics.h>
#include<string>
#include<iostream>
#include<windows.h>
#include<vector>
#include<cmath>
// why nothing happen??

// ����ͼ�����
#pragma comment(lib, "MSIMG32.LIB")

const double PI = 3.1415926;

double PLAYER_SPEED = 5.0;// ����ƶ��ٶ�
POINT player_position = { 500,500 };// ��ҳ�ʼλ��
int sun_count = 50; // ��ʼ��������

inline void putimage_alpha(int x, int y, IMAGE* img) {
    // ͸�����ƺ�����ʹ��AlphaBlend֧��alphaͨ��
    // ԭ�򣿣���<--- AlphaBlend ���ڻ��Դͼ���Ŀ��ͼ�����ɫ�����ǵ�alphaͨ������ʵ��͸�����͸��Ч��
    int w = img->getwidth();
    int h = img->getheight();
    AlphaBlend(GetImageHDC(NULL), x, y, w, h,
        GetImageHDC(img), 0, 0, w, h, 
        { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA });
}

// Forward declarations
class Atlas;
class Animation;
class Plant;
class AttackPlant;
class DefensePlant;
class ResourcePlant;
class Bullet;
class Sun;

class Atlas {
    /// ͼƬ�࣬���ڼ��غ͹���ͼƬ��Դ
    // ��Ҫ����·����ͼƬ����
	// ��Animation��ǰ����
public:
    std::vector<IMAGE*> frame_list;//������ָ̬������
    Atlas(LPCTSTR path, int num) {
        // ���캯��
        // ��ȡ�ļ��ķ�ʽ�ǻ����ļ������Ĺ��ɵģ������Ҫ�����ʽ�����ļ���
        //���廹��Ҫ�����
        TCHAR path_file[256];// �洢ÿ��ͼƬ��·��
        for (size_t i = 0; i < num; i++) {
            _stprintf_s(path_file, path, i); //��·������path_file������
            IMAGE* picture = new IMAGE;// ����IMAGEָ�� picture�����ڴ��棬֮�����loadimage
            loadimage(picture, path_file);//����·��������ͼƬ
            // �����·����һ����ʽ���ַ�����%d�ᱻ�滻Ϊi��ֵ
            frame_list.push_back(picture);// ָ����붯̬������
        }
    }
    ~Atlas() {
        // ��������,�ͷ�ͼƬ��Դ
        for (size_t i = 0; i < frame_list.size(); i++) {
            delete frame_list[i];// �ͷŶ�̬�����е�ָ��
        }
    }
};

class Animation {
	// �����࣬���ڴ�����֡���л�����ʾ
	// ��Ҫ����Atlas����ָ���֡���ʱ��
private:
    Atlas* anim_atlas; // ����֡����
    int internal; // ֡���ʱ��
    int idx_frame = 0; // ��ǰ֡����
    int timer = 0; // ��ʱ��

public:
    Animation(Atlas* atlas, int jiange) : anim_atlas(atlas), internal(jiange) {
        // ���캯��
        //anim_atlas��atlas��Ϊָ��
    }
    ~Animation() {}
    void showimage(int x, int y, int delta) {
        timer += delta;
        if (timer >= internal) {
            // ʹ�ý����ò����� + ��Ա����
            idx_frame = (idx_frame + 1) % anim_atlas->frame_list.size();
            timer = 0;
        }
        // ͨ��ָ���������֡����
        putimage_alpha(x, y, anim_atlas->frame_list[idx_frame]);
    }
};

// ֲ�����
class Plant {
protected:
    int hp;                     // ����ֵ
    POINT position;             // λ��
    int cost;                   // ��ֲ�ɱ�
    Animation* anim;            // ��������
    bool is_alive;              // ���״̬

public:
    Plant(int init_hp, POINT pos, int init_cost, Atlas* atlas, int frame_interval) 
        : hp(init_hp), position(pos), cost(init_cost), is_alive(true) {
        anim = new Animation(atlas, frame_interval);
    }

    virtual ~Plant() {
        if (anim) delete anim;
    }

    // ���麯����������������ʵ��
    virtual void Update(int delta) = 0;
    
    // �������Է���
    int GetHP() const { return hp; }
    POINT GetPosition() const { return position; }
    int GetCost() const { return cost; }
    bool IsAlive() const { return is_alive; }

    // ���˺�����
    virtual void TakeDamage(int damage) {
        hp -= damage;
        if (hp <= 0) {
            hp = 0;
            is_alive = false;
        }
    }

    // ��Ⱦֲ��
    virtual void Draw() {
        if (is_alive && anim) {
            anim->showimage(position.x, position.y, 1000/144);  // ʹ�ù̶�֡��
        }
    }
};

// ������ֲ�����
class AttackPlant : public Plant {
protected:
    int attack_power;      // ������
    int attack_range;      // ������Χ
    int attack_interval;   // �������
    int attack_timer;      // ������ʱ��

public:
    AttackPlant(int init_hp, POINT pos, int init_cost, Atlas* atlas, 
                int frame_interval, int power, int range, int interval)
        : Plant(init_hp, pos, init_cost, atlas, frame_interval),
          attack_power(power), attack_range(range), 
          attack_interval(interval), attack_timer(0) {}

    virtual void Update(int delta) override {
        attack_timer += delta;
        if (attack_timer >= attack_interval) {
            Attack();
            attack_timer = 0;
        }
    }

    virtual void Attack() = 0;  // ���麯�����ɾ���Ĺ�����ֲ��ʵ��
};

// ������ֲ�����
class DefensePlant : public Plant {
protected:
    int defense;           // ����ֵ
    bool is_priority;      // �Ƿ����ȱ�����

public:
    DefensePlant(int init_hp, POINT pos, int init_cost, Atlas* atlas, 
                 int frame_interval, int init_defense, bool priority)
        : Plant(init_hp, pos, init_cost, atlas, frame_interval),
          defense(init_defense), is_priority(priority) {}

    virtual void Update(int delta) override {
        // ������ֲ��Ļ��������߼�
    }

    // ��д���˺��������Ƿ���ֵ
    virtual void TakeDamage(int damage) override {
        int actual_damage = damage - defense;
        if (actual_damage > 0) {
            Plant::TakeDamage(actual_damage);
        }
    }

    bool IsPriority() const { return is_priority; }
};

// ��Դ��ֲ�����
class ResourcePlant : public Plant {
protected:
    int resource_rate;     // ��Դ��������
    int resource_amount;   // ÿ�β�������Դ��
    int resource_timer;    // ��Դ������ʱ��

public:
    ResourcePlant(int init_hp, POINT pos, int init_cost, Atlas* atlas,
                 int frame_interval, int rate, int amount)
        : Plant(init_hp, pos, init_cost, atlas, frame_interval),
          resource_rate(rate), resource_amount(amount), resource_timer(0) {}

    void Update(int delta) override {
        resource_timer += delta;
        if (resource_timer >= resource_rate) {
            GenerateResource();
            resource_timer = 0;
        }
    }

    virtual void GenerateResource() = 0;  // ���麯�����ɾ������Դ��ֲ��ʵ��
};

// �ӵ�����
class Bullet {
private:
    Animation* anim;        // ��������ָ��
    double speed;          // �ƶ��ٶ�
    POINT position;        // �ӵ�λ��
    bool is_active;        // �Ƿ񼤻�
    static Atlas* bullet_atlas; // �ӵ���ͼ���������ӵ�����

public:
    Bullet(POINT pos, double init_speed) 
        : position(pos), speed(init_speed), is_active(true){
        anim = new Animation(bullet_atlas, 50);
    }

    virtual ~Bullet() {
        if (anim) delete anim;
    }

    // �����ӵ�λ��
    virtual void Update(int delta) {
        if (!is_active) return;

        // �����ƶ�
        position.x += speed * delta / 1000.0;

        // ����Ƴ���Ļ������
        if (position.x > 1280) {
            is_active = false;
        }
    }

    // �����ӵ�
    virtual void Draw() {
        if (is_active && anim) {
            anim->showimage(position.x, position.y, 1000/144);
        }
    }

    
    // ��ȡ�ӵ�λ��
    POINT GetPosition() const { return position; }
    
    // ����ӵ��Ƿ񼤻�
    bool IsActive() const { return is_active; }
    
    // ͣ���ӵ�
    void Deactivate() { is_active = false; }
};

// ��ͨ�ӵ���
class NormalBullet : public Bullet {
private:
    static IMAGE* bullet_img;  // ��̬ͼƬ����������ͨ�ӵ�����

public:
    NormalBullet(POINT pos, double init_speed)
        : Bullet(pos, init_speed) {
        if (!bullet_img) {
            bullet_img = new IMAGE();
            loadimage(bullet_img, _T("img/normal_bullet.gif"));
        }
    }

    void Draw() override {
        if (IsActive() && bullet_img) {
            putimage_alpha(GetPosition().x, GetPosition().y, bullet_img);
        }
    }

    static void Cleanup() {
        if (bullet_img) {
            delete bullet_img;
            bullet_img = nullptr;
        }
    }
};

// ��ʼ����̬��Ա
IMAGE* NormalBullet::bullet_img = nullptr;

// ��ʼ����̬��Ա
Atlas* Bullet::bullet_atlas = nullptr;

// ������
class Sun {
private:
    POINT position;        // ��ǰλ��
    POINT target;         // Ŀ��λ��
    Animation* anim;      // ����
    bool is_active;       // �Ƿ񼤻�
    const double MOVE_SPEED = 5.0; // �ƶ��ٶ�

public:
    Sun(POINT pos, Atlas* atlas) : position(pos), is_active(true) {
        // �������������λ��
        target = { 68, 2 };
        
        // ��������
        anim = new Animation(atlas, 50);
    }

    ~Sun() {
        if (anim) delete anim;
    }

    void Update(int delta) {
        if (!is_active) return;

        // ����Ŀ��λ��
        double dx = target.x - position.x;
        double dy = target.y - position.y;
        double distance = sqrt(dx*dx + dy*dy);
        
        if (distance < MOVE_SPEED) {
            is_active = false;
            sun_count += 25;  // �ռ�������
        }
        else {
            position.x += (dx / distance) * MOVE_SPEED;
            position.y += (dy / distance) * MOVE_SPEED;
        }
    }

    void Draw() {
        if (is_active && anim) {
            anim->showimage(position.x, position.y, 1000/144);
        }
    }

    bool IsActive() const { return is_active; }
};

// ********************************����ֲ��**************************************//


// ���տ��ࣨ��Դ��ֲ��ʾ����
class Sunflower : public ResourcePlant {
private:
    static Atlas* sun_atlas;  // ���⶯��֡����
    std::vector<Sun*> suns;   // ����������

public:
    Sunflower(POINT pos)
        : ResourcePlant(100, pos, 50, new Atlas(_T("img/sunflower_%d.png"), 17), 
                       100, 2000, 25) {
        if (!sun_atlas) {
            sun_atlas = new Atlas(_T("img/sun_%d.png"), 28);
        }
    }

    ~Sunflower() {
        for (auto sun : suns) {
            delete sun;
        }
        suns.clear();
    }

    void GenerateResource() override {
        // �����տ��Ҳ�20���ش���������
        POINT sun_pos = position;
        sun_pos.x += 40;  // ���տ�����
        sun_pos.y += 40;
        sun_pos.x += 20;  // ������20����
        suns.push_back(new Sun(sun_pos, sun_atlas));
    }

    void Update(int delta) override {
        ResourcePlant::Update(delta);

        // ������������
        for (auto sun : suns) {
            sun->Update(delta);
        }

        // ɾ������Ծ������
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

    void Draw() override {
        Plant::Draw();
        // ������������
        for (auto sun : suns) {
            sun->Draw();
        }
    }
};

// ��ʼ����̬��Ա
Atlas* Sunflower::sun_atlas = nullptr;

// �㶹�����ࣨ������ֲ��ʾ����
class Peashooter : public AttackPlant {
public:
    Peashooter(POINT pos)
        : AttackPlant(100, pos, 100, new Atlas(_T("img/peashooter_%d.png"), 12),
                     100, 20, 400, 1500) {}

    void Attack() override {
        // TODO: ʵ�ַ����㶹���߼�
    }
};

// ���ǽ�ࣨ������ֲ��ʾ����
class WallNut : public DefensePlant {
public:
    WallNut(POINT pos)
        : DefensePlant(400, pos, 50, new Atlas(_T("img/wallnut_%d.png"), 15),
                      150, 20, true) {}
};

//******************Atlas����********************//
// ����ȫ�ֵ�Atlas����
Atlas* atlas_player_left = new Atlas(_T("img/player_left_%d.png"), 6);
Atlas* atlas_player_right = new Atlas(_T("img/player_right_%d.png"), 6);











//******************Atlas����********************//

int main() {
    initgraph(1280, 720); // ��������
    
    // ����������ʽ
    LOGFONT font;
    gettextstyle(&font);
    font.lfHeight = 30;  // ��������߶�
    _tcscpy_s(font.lfFaceName, _T("Arial")); // ��������
    font.lfQuality = ANTIALIASED_QUALITY;    // ���ÿ����
    settextstyle(&font);                     // ����������ʽ
    setbkmode(TRANSPARENT);                  // ���ñ���͸��

    // ������������
    Animation* anim_player_left = new Animation(atlas_player_left, 45);
    Animation* anim_player_right = new Animation(atlas_player_right, 45);
    
    bool running = true; // ��Ϸ���б�־
    ExMessage msg; // ��Ϣ�ṹ�� ����֮������Ϣ
    
    IMAGE img_background; // ����ͼƬ��
    IMAGE test_img; // ����ͼƬ��
    IMAGE sun_back; // ����ͼƬ��
	IMAGE tombstone; // Ĺ��ͼƬ��

    // ֲ�����
    int selected_plant = 0;  // ��ǰѡ�е�ֲ�����ͣ�0��ʾδѡ�У�
    std::vector<Plant*> plants;  // �洢���з��õ�ֲ��

    // ��ʼ���ƶ����Ʊ���
    bool facing_left = true; // ��ɫ����trueΪ��falseΪ��
    bool moving_up = false;
    bool moving_down = false;
    bool moving_left = false;
    bool moving_right = false;

    loadimage(&img_background, _T("img/background.png"));// ���ر���ͼƬ
    //loadimage(&test_img, _T("img/SunFlower1.gif"));// ���ز���ͼƬ
    loadimage(&sun_back, _T("img/sun_back.png"));// ����������
	loadimage(&tombstone, _T("img/tombstone.png"), 150, 150);// ����Ĺ�� ��ȣ�150 �߶ȣ�150


    BeginBatchDraw();// ����������ͼ
    while (running) {
        DWORD startTime = GetTickCount(); // ��ʡʱ�䲿��

        while (peekmessage(&msg)) {
            // ����������
            switch (msg.message) {
            case WM_KEYDOWN:
                switch (msg.vkcode) {  // msg.wParam ��msg.vkcode��������ʲô��
                case '1':  // ѡ�����տ�
                    selected_plant = 1;
                    break;
                case '2':  // ѡ���㶹����
                    selected_plant = 2;
                    break;
                case '3':  // ѡ����ǽ
                    selected_plant = 3;
                    break;
                case VK_UP:
                case 'W':          // ���� W ��
                    moving_up = true;
                    break;
                case VK_DOWN:
                case 'S':          // ���� S ��
                    moving_down = true;
                    break;
                case VK_LEFT:
                case 'A':          // ���� A ��
                    moving_left = true;
                    facing_left = true;
                    break;
                case VK_RIGHT:
                case 'D':          // ���� D ��
                    moving_right = true;
                    facing_left = false;
                    break;
                case VK_ESCAPE:
                    running = false;
                    break;
                }
                break;

            case WM_KEYUP:
                switch (msg.vkcode) {
                case '1':
                case '2':
                case '3':
                    selected_plant = 0;  // �ͷ����ּ�ʱȡ��ѡ��
                    break;
                case VK_UP:
                case 'W':          // �ͷ� W ��ʱֹͣ����
                    moving_up = false;
                    break;
                case VK_DOWN:
                case 'S':          // �ͷ� S ��ʱֹͣ����
                    moving_down = false;
                    break;
                case VK_LEFT:
                case 'A':          // �ͷ� A ��ʱֹͣ����
                    moving_left = false;
                    break;
                case VK_RIGHT:
                case 'D':          // �ͷ� D ��ʱֹͣ����
                    moving_right = false;
                    break;
                }
                break;

            case WM_LBUTTONDOWN:
                if (selected_plant > 0) {
                    // ��ȡ�����λ��
                    POINT click_pos = { msg.x, msg.y };
                    
                    // ����ѡ�񴴽���Ӧ��ֲ��
                    Plant* new_plant = nullptr;
                    int cost = 0;
                    
                    switch (selected_plant) {
                    case 1:
                        cost = 50;  // ���տ��ɱ�
                        if (sun_count >= cost) {
                            new_plant = new Sunflower(click_pos);
                            sun_count -= cost;
                        }
                        break;
                    case 2:
                        cost = 100; // �㶹���ֳɱ�
                        if (sun_count >= cost) {
                            new_plant = new Peashooter(click_pos);
                            sun_count -= cost;
                        }
                        break;
                    case 3:
                        cost = 50;  // ���ǽ�ɱ�
                        if (sun_count >= cost) {
                            new_plant = new WallNut(click_pos);
                            sun_count -= cost;
                        }
                        break;
                    }
                    
                    if (new_plant) {
                        plants.push_back(new_plant);
                    }
                }
                break;
            }
        }
        
        // ���½�ɫλ��
        if (moving_up)
            player_position.y -= PLAYER_SPEED;
        if (moving_down)
            player_position.y += PLAYER_SPEED;
        if (moving_left)
            player_position.x -= PLAYER_SPEED;
        if (moving_right)
            player_position.x += PLAYER_SPEED;

        // �߽���
        if (player_position.x < 0) player_position.x = 0;
        if (player_position.y < 0) player_position.y = 0;
        if (player_position.x > 1280 - 80) player_position.x = 1280 - 70; // �����ɫ���Ϊ80
        if (player_position.y > 720 - 80) player_position.y = 720 - 80;   // �����ɫ�߶�Ϊ80

        // ��������ֲ��
        DWORD delta = 1000 / 144;
        for (auto plant : plants) {
            plant->Update(delta);
        }

        cleardevice();// �����Ļ��Ȼ�������֡
        // �·�ΪҪ������ȥ������

        putimage(0, 0, &img_background);//���ñ���ͼƬ�ڣ�0��0��
        //putimage_alpha(0, 0, &test_img);//���ò���ͼƬ�ڣ�0��0��
        putimage_alpha(0, 0, &sun_back);
        putimage_alpha(1000, 50, &tombstone);
        // ��������ֲ��
        for (auto plant : plants) {
            plant->Draw();
        }

        // ������������
        setfillcolor(RGB(255, 215, 0));  // ����������ɫΪ��ɫ
        settextcolor(RGB(0, 0, 0));      // ����������ɫΪ��ɫ
        TCHAR sun_text[20];
        _stprintf_s(sun_text, _T("%d"), sun_count);
        RECT text_rect = { 68, 2, 120, 32 }; // �����ı���ʾ����
        drawtext(sun_text, &text_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE); // ���ж�����ʾ

        // ʹ��Animation����ʾ��ɫ����
        if (moving_left || moving_right || moving_up || moving_down) {
            //******************ֻ���ƶ�ʱ���¶���****************//
            if (facing_left) {
                anim_player_left->showimage(player_position.x, player_position.y, delta);
            } else {
                anim_player_right->showimage(player_position.x, player_position.y, delta);
            }
        } else {
            // ��ֹʱ��ʾ��һ֡
            if (facing_left) {
                putimage_alpha(player_position.x, player_position.y, atlas_player_left->frame_list[0]);
            } else {  
                putimage_alpha(player_position.x, player_position.y, atlas_player_right->frame_list[0]);
            }
        }

        // �����ֲ�ﱻѡ�У���ʾ��ǰѡ�е�ֲ������
        if (selected_plant > 0) {
            TCHAR s[20];
            _stprintf_s(s, _T("Selected: %d"), selected_plant);
            outtextxy(10, 10, s);
        }

        FlushBatchDraw();// ˢ����Ļ����ʾ�»��Ƶ�����

        // ����֡��
        DWORD endTime = GetTickCount();
        DWORD delta_time = endTime - startTime;
        if (delta_time < 1000 / 144) {
            Sleep(1000 / 144 - delta_time);
        }
    }

    // ������Դ
    for (auto plant : plants) {
        delete plant;
    }
    plants.clear();

    delete anim_player_left;
    delete anim_player_right;
    delete atlas_player_left;
    delete atlas_player_right;

    closegraph();// �ر�ͼ�δ���
    return 0;
}