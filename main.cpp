#include <graphics.h>      // EasyXͼ�ο�
#include <string>
#include <iostream>
#include <windows.h>      // Windows API
#include <vector>         // ��׼ģ���vector����
#include <cmath>          // ��ѧ������

// �����Զ������ͷ�ļ�
#include "Animation.h"        // ����ϵͳ
#include "Plant.h"           // ֲ�����
#include "SpecificPlants.h"  // ����ֲ����
#include "Bullet.h"          // �ӵ�ϵͳ
#include "Sun.h"             // ����ϵͳ
#include "Zombie.h"          // ��ʬϵͳ
#include "BrainBase.h"       // ���Ի�����

// ����Windowsͼ����⣬����֧��͸��ͼƬ
#pragma comment(lib, "MSIMG32.LIB")

// ȫ�ֳ����ͱ���
const double PI = 3.1415926;         // Բ���ʳ���
double PLAYER_SPEED = 5.0;           // ����ƶ��ٶ�
POINT player_position = { 500,500 }; // ��ҳ�ʼλ��
int sun_count = 50;                  // ��ʼ��������

// ����ȫ�ֵ���Ҷ���ͼ������
Atlas* atlas_player_left = new Atlas(_T("img/player_left_%d.png"), 6);   // ���򶯻�֡
Atlas* atlas_player_right = new Atlas(_T("img/player_right_%d.png"), 6);  // ���򶯻�֡

int main() {
    initgraph(1280, 720); // ��ʼ��ͼ�δ��ڣ����ô�СΪ1280x720

    // ����������ʾ��ʽ
    LOGFONT font;
    gettextstyle(&font);
    font.lfHeight = 30;                         // ����߶�
    _tcscpy_s(font.lfFaceName, _T("Arial"));   // ��������
    font.lfQuality = ANTIALIASED_QUALITY;       // �����
    settextstyle(&font);                        // Ӧ����������
    setbkmode(TRANSPARENT);                     // ���ֱ���͸��

    // ������Ҷ�������
    Animation* anim_player_left = new Animation(atlas_player_left, 45);   // ���򶯻�
    Animation* anim_player_right = new Animation(atlas_player_right, 45); // ���򶯻�

    // ��Ϸ״̬��־
    bool running = true;         // ��Ϸ���б�־
    bool game_started = false;   // ��Ϸ�Ƿ�ʼ���Ƿ��ѷ��ô��ԣ�
    ExMessage msg;              // ��Ϣ�ṹ�壬���ڴ����û�����

    // ������Ϸ��Դ
    IMAGE img_background;        // ����ͼƬ
    IMAGE sun_back;             // �������������
    IMAGE tombstone;            // Ĺ��ͼƬ

    // ������Ϸ����
    BrainBase* brain = new BrainBase();    // �������Ի���
    std::vector<Plant*> plants;            // ֲ������
    int selected_plant = 0;                // ��ǰѡ�е�ֲ�����ͣ�0��ʾδѡ�У�

    // ����ƶ����Ʊ���
    bool facing_left = true;    // ��ɫ����trueΪ��falseΪ�ң�
    bool moving_up = false;     // �����ƶ���־
    bool moving_down = false;   // �����ƶ���־
    bool moving_left = false;   // �����ƶ���־
    bool moving_right = false;  // �����ƶ���־

    // ����ͼƬ��Դ
    loadimage(&img_background, _T("img/background.png"));            // ���ر���
    loadimage(&sun_back, _T("img/sun_back.png"));                   // ����������
    loadimage(&tombstone, _T("img/tombstone.png"), 150, 150);       // ����Ĺ��

    // ��ʼ����ʬϵͳ
    POINT tombstone_pos = { 1000, 50 };        // ����Ĺ������ʬ���ɵ㣩λ��
    ZombieSpawner spawner(tombstone_pos);      // ������ʬ������
    std::vector<Zombie*> zombies;              // ��ʬ����

    BeginBatchDraw(); // ��ʼ������ͼ����ֹ��˸

    // ��Ϸ��ʼǰ�Ĵ��Է��ý׶�
    settextcolor(WHITE);
    settextstyle(40, 0, _T("Arial"));
    outtextxy(400, 300, _T("�����Ļ���ô��Ի���"));
    FlushBatchDraw();

    // �ȴ���ҷ��ô���
    while (!game_started && running) {
        if (peekmessage(&msg)) {
            if (msg.message == WM_LBUTTONDOWN) {
                POINT click_pos = { msg.x, msg.y };
                // ȷ�����Է����ں���λ��
                if (click_pos.x > 100 && click_pos.x < 1180 && 
                    click_pos.y > 100 && click_pos.y < 620) {
                    brain->SetPosition(click_pos);
                    game_started = true;
                }
            }
            else if (msg.message == WM_KEYDOWN && msg.vkcode == VK_ESCAPE) {
                running = false;
            }
        }
    }

    // ����Ϸѭ��
    while (running) {
        DWORD startTime = GetTickCount(); // ��¼֡��ʼʱ��

        // �����Ϸ�Ƿ�ʧ��
        if (!brain->IsAlive()) {
            cleardevice();
            settextcolor(RED);
            settextstyle(60, 0, _T("Arial"));
            outtextxy(480, 300, _T("��Ϸ����!"));
            FlushBatchDraw();
            Sleep(2000);
            running = false;
            continue;
        }

        // �����û�����
        while (peekmessage(&msg)) {
            switch (msg.message) {
                case WM_KEYDOWN:    // ���������¼�
                    switch (msg.vkcode) {
                        case '1': selected_plant = 1; break;  // ѡ�����տ�
                        case '2': selected_plant = 2; break;  // ѡ���㶹����
                        case '3': selected_plant = 3; break;  // ѡ����ǽ
                        case 'W': case VK_UP: moving_up = true; break;
                        case 'S': case VK_DOWN: moving_down = true; break;
                        case 'A': case VK_LEFT: 
                            moving_left = true; 
                            facing_left = true; 
                            break;
                        case 'D': case VK_RIGHT: 
                            moving_right = true; 
                            facing_left = false; 
                            break;
                        case VK_ESCAPE: running = false; break;
                    }
                    break;

                case WM_KEYUP:      // �����ͷ��¼�
                    switch (msg.vkcode) {
                        case '1': case '2': case '3': selected_plant = 0; break;
                        case 'W': case VK_UP: moving_up = false; break;
                        case 'S': case VK_DOWN: moving_down = false; break;
                        case 'A': case VK_LEFT: moving_left = false; break;
                        case 'D': case VK_RIGHT: moving_right = false; break;
                    }
                    break;

                case WM_LBUTTONDOWN:    // ����������¼�
                    if (selected_plant > 0) {
                        POINT click_pos = { msg.x, msg.y };
                        Plant* new_plant = nullptr;
                        int cost = 0;

                        // ����ѡ�񴴽���Ӧ��ֲ��
                        switch (selected_plant) {
                            case 1: // ���տ�
                                cost = 50;
                                if (sun_count >= cost) {
                                    new_plant = new Sunflower(click_pos);
                                    sun_count -= cost;
                                }
                                break;
                            case 2: // �㶹����
                                cost = 100;
                                if (sun_count >= cost) {
                                    new_plant = new Peashooter(click_pos);
                                    sun_count -= cost;
                                }
                                break;
                            case 3: // ���ǽ
                                cost = 50;
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

        // ������Ϸ״̬
        DWORD delta = 1000 / 144;  // ����֡���ʱ��

        // �������λ��
        if (moving_up) player_position.y -= PLAYER_SPEED;
        if (moving_down) player_position.y += PLAYER_SPEED;
        if (moving_left) player_position.x -= PLAYER_SPEED;
        if (moving_right) player_position.x += PLAYER_SPEED;

        // ��ұ߽���
        if (player_position.x < 0) player_position.x = 0;
        if (player_position.y < 0) player_position.y = 0;
        if (player_position.x > 1280 - 80) player_position.x = 1280 - 81;
        if (player_position.y > 720 - 80) player_position.y = 720 - 81;

        // ���½�ʬϵͳ
        if (Zombie* new_zombie = spawner.Update(delta)) {
            zombies.push_back(new_zombie);
        }

        // �������н�ʬ
        for (auto zombie : zombies) {
            zombie->Update(delta, plants, brain);
        }

        // ���������Ľ�ʬ
        zombies.erase(
            std::remove_if(zombies.begin(), zombies.end(),
                [](Zombie* zombie) {
                    if (!zombie->IsAlive()) {
                        delete zombie;
                        return true;
                    }
                    return false;
                }
            ),
            zombies.end()
        );

        // ����ֲ��
        for (auto plant : plants) {
            plant->Update(delta);
        }

        // ������Ϸ����
        cleardevice();  // �����Ļ

        // ���Ʊ�����
        putimage(0, 0, &img_background);
        putimage_alpha(0, 0, &sun_back);
        putimage_alpha(1000, 50, &tombstone);

        // ������Ϸ����
        brain->Draw();              // ���ƴ��Ի���
        for (auto plant : plants) { // ����ֲ��
            plant->Draw();
        }
        for (auto zombie : zombies) { // ���ƽ�ʬ
            zombie->Draw();
        }

        // ����UI��
        // ��������
        setfillcolor(RGB(255, 215, 0));
        settextcolor(RGB(0, 0, 0));
        TCHAR sun_text[20];
        _stprintf_s(sun_text, _T("%d"), sun_count);
        RECT text_rect = { 68, 2, 120, 32 };
        drawtext(sun_text, &text_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        // ����Ѫ��
        TCHAR hp_text[20];
        _stprintf_s(hp_text, _T("����Ѫ��: %d"), brain->GetHP());
        outtextxy(10, 50, hp_text);

        // ��ʬ������������Ϣ��
        TCHAR zombie_text[20];
        _stprintf_s(zombie_text, _T("��ʬ����: %d"), (int)zombies.size());
        outtextxy(10, 90, zombie_text);

        // ������ҽ�ɫ
        if (moving_left || moving_right || moving_up || moving_down) {
            // �ƶ�ʱ���Ŷ���
            if (facing_left) {
                anim_player_left->showimage(player_position.x, player_position.y, delta);
            }
            else {
                anim_player_right->showimage(player_position.x, player_position.y, delta);
            }
        }
        else {
            // ��ֹʱ��ʾ��һ֡
            if (facing_left) {
                putimage_alpha(player_position.x, player_position.y, atlas_player_left->frame_list[0]);
            }
            else {
                putimage_alpha(player_position.x, player_position.y, atlas_player_right->frame_list[0]);
            }
        }

        // ��ʾ��ǰѡ�е�ֲ��
        if (selected_plant > 0) {
            TCHAR s[20];
            _stprintf_s(s, _T("Selected: %d"), selected_plant);
            outtextxy(10, 10, s);
        }

        FlushBatchDraw(); // ������Ļ��ʾ

        // ����֡��
        DWORD endTime = GetTickCount();
        DWORD delta_time = endTime - startTime;
        if (delta_time < 1000 / 144) {
            Sleep(1000 / 144 - delta_time);
        }
    }

    // ������Դ
    // ����ʬ
    for (auto zombie : zombies) {
        delete zombie;
    }
    zombies.clear();

    // ����ֲ��
    for (auto plant : plants) {
        delete plant;
    }
    plants.clear();

    // ����������Դ
    delete brain;
    delete anim_player_left;
    delete anim_player_right;
    delete atlas_player_left;
    delete atlas_player_right;

    closegraph(); // �ر�ͼ�δ���
    return 0;
}
