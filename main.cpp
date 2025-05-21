#include <graphics.h>
#include <string>
#include <iostream>
#include <windows.h>
#include <vector>
#include <cmath>

#include "Animation.h"
#include "Plant.h"
#include "SpecificPlants.h"
#include "Bullet.h"
#include "Sun.h"
#include "Zombie.h"
#include "BrainBase.h"

// ����ͼ�����
#pragma comment(lib, "MSIMG32.LIB")

const double PI = 3.1415926;
double PLAYER_SPEED = 5.0;
POINT player_position = { 500,500 };
int sun_count = 50;

// ����ȫ�ֵ�Atlas����
Atlas* atlas_player_left = new Atlas(_T("img/player_left_%d.png"), 6);
Atlas* atlas_player_right = new Atlas(_T("img/player_right_%d.png"), 6);

int main() {
    initgraph(1280, 720); // ��������

    // ����������ʽ
    LOGFONT font;
    gettextstyle(&font);
    font.lfHeight = 30; // ��������߶�
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
    int selected_plant = 0; // ��ǰѡ�е�ֲ�����ͣ�0��ʾδѡ�У�
    std::vector<Plant*> plants; // �洢���з��õ�ֲ��

    // ��ʼ���ƶ����Ʊ���
    bool facing_left = true; // ��ɫ����trueΪ��falseΪ��
    bool moving_up = false;
    bool moving_down = false;
    bool moving_left = false;
    bool moving_right = false;

    loadimage(&img_background, _T("img/background.png")); // ���ر���ͼƬ
    //loadimage(&test_img, _T("img/SunFlower1.gif"));// ���ز���ͼƬ
    loadimage(&sun_back, _T("img/sun_back.png")); // ����������
    loadimage(&tombstone, _T("img/tombstone.png"), 150, 150); // ����Ĺ�� ��ȣ�150 �߶ȣ�150


    BeginBatchDraw(); // ����������ͼ
    while (running) {
        DWORD startTime = GetTickCount(); // ��ʡʱ�䲿��

        while (peekmessage(&msg)) {
            // ����������
            switch (msg.message) {
            case WM_KEYDOWN:
                switch (msg.vkcode) { // msg.wParam ��msg.vkcode��������ʲô��
                case '1': // ѡ�����տ�
                    selected_plant = 1;
                    break;
                case '2': // ѡ���㶹����
                    selected_plant = 2;
                    break;
                case '3': // ѡ����ǽ
                    selected_plant = 3;
                    break;
                case VK_UP:
                case 'W': // ���� W ��
                    moving_up = true;
                    break;
                case VK_DOWN:
                case 'S': // ���� S ��
                    moving_down = true;
                    break;
                case VK_LEFT:
                case 'A': // ���� A ��
                    moving_left = true;
                    facing_left = true;
                    break;
                case VK_RIGHT:
                case 'D': // ���� D ��
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
                    selected_plant = 0; // �ͷ����ּ�ʱȡ��ѡ��
                    break;
                case VK_UP:
                case 'W': // �ͷ� W ��ʱֹͣ����
                    moving_up = false;
                    break;
                case VK_DOWN:
                case 'S': // �ͷ� S ��ʱֹͣ����
                    moving_down = false;
                    break;
                case VK_LEFT:
                case 'A': // �ͷ� A ��ʱֹͣ����
                    moving_left = false;
                    break;
                case VK_RIGHT:
                case 'D': // �ͷ� D ��ʱֹͣ����
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
                        cost = 50; // ���տ��ɱ�
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
                        cost = 50; // ���ǽ�ɱ�
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

        cleardevice(); // �����Ļ��Ȼ�������֡
//*****************************�·�ΪҪ������ȥ������********************************//

        DWORD delta = 1000 / 144;
        for (auto plant : plants) {
            plant->Update(delta);
        }
        putimage(0, 0, &img_background); //���ñ���ͼƬ�ڣ�0��0��
        //putimage_alpha(0, 0, &test_img);//���ò���ͼƬ�ڣ�0��0��
        putimage_alpha(0, 0, &sun_back);
        putimage_alpha(1000, 50, &tombstone);
        // ��������ֲ��
        for (auto plant : plants) {
            plant->Draw();
        }

        // ������������
        setfillcolor(RGB(255, 215, 0)); // ����������ɫΪ��ɫ
        settextcolor(RGB(0, 0, 0));     // ����������ɫΪ��ɫ
        TCHAR sun_text[20];
        _stprintf_s(sun_text, _T("%d"), sun_count);
        RECT text_rect = { 68, 2, 120, 32 }; // �����ı���ʾ����
        drawtext(sun_text, &text_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE); // ���ж�����ʾ

        // ʹ��Animation����ʾ��ɫ����
        if (moving_left || moving_right || moving_up || moving_down) {
            //******************ֻ���ƶ�ʱ���¶���****************//
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

        // �����ֲ�ﱻѡ�У���ʾ��ǰѡ�е�ֲ������
        if (selected_plant > 0) {
            TCHAR s[20];
            _stprintf_s(s, _T("Selected: %d"), selected_plant);
            outtextxy(10, 10, s);
        }

        FlushBatchDraw(); // ˢ����Ļ����ʾ�»��Ƶ�����

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


    closegraph(); // �ر�ͼ�δ���
    return 0;
}
