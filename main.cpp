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

// 链接图像处理库
#pragma comment(lib, "MSIMG32.LIB")

const double PI = 3.1415926;
double PLAYER_SPEED = 5.0;
POINT player_position = { 500,500 };
int sun_count = 50;

// 创建全局的Atlas对象
Atlas* atlas_player_left = new Atlas(_T("img/player_left_%d.png"), 6);
Atlas* atlas_player_right = new Atlas(_T("img/player_right_%d.png"), 6);

int main() {
    initgraph(1280, 720); // 创建窗口

    // 设置文字样式
    LOGFONT font;
    gettextstyle(&font);
    font.lfHeight = 30; // 设置字体高度
    _tcscpy_s(font.lfFaceName, _T("Arial")); // 设置字体
    font.lfQuality = ANTIALIASED_QUALITY;    // 设置抗锯齿
    settextstyle(&font);                     // 设置文字样式
    setbkmode(TRANSPARENT);                  // 设置背景透明

    // 创建动画对象
    Animation* anim_player_left = new Animation(atlas_player_left, 45);
    Animation* anim_player_right = new Animation(atlas_player_right, 45);

    bool running = true; // 游戏运行标志
    bool game_started = false; // 游戏是否已经开始（是否已放置大脑）
    ExMessage msg; // 消息结构体

    IMAGE img_background;
    IMAGE sun_back;
    IMAGE tombstone;

    // 创建大脑基地
    BrainBase* brain = new BrainBase();

    // 植物管理
    int selected_plant = 0;
    std::vector<Plant*> plants;

    // 初始化移动控制变量
    bool facing_left = true;
    bool moving_up = false;
    bool moving_down = false;
    bool moving_left = false;
    bool moving_right = false;

    loadimage(&img_background, _T("img/background.png"));
    loadimage(&sun_back, _T("img/sun_back.png"));
    loadimage(&tombstone, _T("img/tombstone.png"), 150, 150);

    // 初始化僵尸生成器和容器
    POINT tombstone_pos = { 1000, 50 };  // 墓碑位置
    ZombieSpawner spawner(tombstone_pos);
    std::vector<Zombie*> zombies;

    BeginBatchDraw();

    // 大脑放置阶段
    settextcolor(WHITE);
    settextstyle(40, 0, _T("Arial"));
    outtextxy(400, 300, _T("点击屏幕放置大脑基地"));
    FlushBatchDraw();

    // 等待玩家放置大脑
    while (!game_started && running) {
        if (peekmessage(&msg)) {
            if (msg.message == WM_LBUTTONDOWN) {
                POINT click_pos = { msg.x, msg.y };
                // 确保大脑不会被放置在屏幕边缘
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

    // 主游戏循环
    while (running) {
        DWORD startTime = GetTickCount();

        // 检查游戏是否失败
        if (!brain->IsAlive()) {
            // 显示游戏结束画面
            cleardevice();
            settextcolor(RED);
            settextstyle(60, 0, _T("Arial"));
            outtextxy(480, 300, _T("游戏结束!"));
            FlushBatchDraw();
            Sleep(2000); // 显示2秒
            running = false;
            continue;
        }

        while (peekmessage(&msg)) {
            // 按键处理部分
            switch (msg.message) {
            case WM_KEYDOWN:
                switch (msg.vkcode) { // msg.wParam 和msg.vkcode的区别是什么？
                case '1': // 选择向日葵
                    selected_plant = 1;
                    break;
                case '2': // 选择豌豆射手
                    selected_plant = 2;
                    break;
                case '3': // 选择坚果墙
                    selected_plant = 3;
                    break;
                case VK_UP:
                case 'W': // 新增 W 键
                    moving_up = true;
                    break;
                case VK_DOWN:
                case 'S': // 新增 S 键
                    moving_down = true;
                    break;
                case VK_LEFT:
                case 'A': // 新增 A 键
                    moving_left = true;
                    facing_left = true;
                    break;
                case VK_RIGHT:
                case 'D': // 新增 D 键
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
                    selected_plant = 0; // 释放数字键时取消选择
                    break;
                case VK_UP:
                case 'W': // 释放 W 键时停止上移
                    moving_up = false;
                    break;
                case VK_DOWN:
                case 'S': // 释放 S 键时停止下移
                    moving_down = false;
                    break;
                case VK_LEFT:
                case 'A': // 释放 A 键时停止左移
                    moving_left = false;
                    break;
                case VK_RIGHT:
                case 'D': // 释放 D 键时停止右移
                    moving_right = false;
                    break;
                }
                break;

            case WM_LBUTTONDOWN:
                if (selected_plant > 0) {
                    // 获取鼠标点击位置
                    POINT click_pos = { msg.x, msg.y };

                    // 根据选择创建对应的植物
                    Plant* new_plant = nullptr;
                    int cost = 0;

                    switch (selected_plant) {
                    case 1:
                        cost = 50; // 向日葵成本
                        if (sun_count >= cost) {
                            new_plant = new Sunflower(click_pos);
                            sun_count -= cost;
                        }
                        break;
                    case 2:
                        cost = 100; // 豌豆射手成本
                        if (sun_count >= cost) {
                            new_plant = new Peashooter(click_pos);
                            sun_count -= cost;
                        }
                        break;
                    case 3:
                        cost = 50; // 坚果墙成本
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

        // 更新角色位置
        if (moving_up)
            player_position.y -= PLAYER_SPEED;
        if (moving_down)
            player_position.y += PLAYER_SPEED;
        if (moving_left)
            player_position.x -= PLAYER_SPEED;
        if (moving_right)
            player_position.x += PLAYER_SPEED;

        // 边界检测
        if (player_position.x < 0) player_position.x = 0;
        if (player_position.y < 0) player_position.y = 0;
        if (player_position.x > 1280 - 80) player_position.x = 1280 - 81; // 假设角色宽度为80
        if (player_position.y > 720 - 80) player_position.y = 720 - 81;   // 假设角色高度为80

        // 更新僵尸生成器
        if (Zombie* new_zombie = spawner.Update(1000 / 144)) {
            zombies.push_back(new_zombie);
        }

        // 更新所有僵尸
        for (auto zombie : zombies) {
            zombie->Update(1000 / 144, plants, brain);
        }

        // 删除死亡的僵尸
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

        // 绘制游戏画面
        cleardevice();
        
        DWORD delta = 1000 / 144;
        
        // 绘制背景和UI
        putimage(0, 0, &img_background);
        putimage_alpha(0, 0, &sun_back);
        putimage_alpha(1000, 50, &tombstone);

        // 更新和绘制大脑
        brain->Draw();

        // 更新和绘制植物
        for (auto plant : plants) {
            plant->Update(delta);
            plant->Draw();
        }

        // 绘制僵尸
        for (auto zombie : zombies) {
            zombie->Draw();
        }

        // 绘制阳光数量
        setfillcolor(RGB(255, 215, 0));
        settextcolor(RGB(0, 0, 0));
        TCHAR sun_text[20];
        _stprintf_s(sun_text, _T("%d"), sun_count);
        RECT text_rect = { 68, 2, 120, 32 };
        drawtext(sun_text, &text_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        // 绘制大脑血量
        TCHAR hp_text[20];
        _stprintf_s(hp_text, _T("大脑血量: %d"), brain->GetHP());
        outtextxy(10, 50, hp_text);

        // 绘制僵尸数量（调试用）
        TCHAR zombie_text[20];
        _stprintf_s(zombie_text, _T("僵尸数量: %d"), (int)zombies.size());
        outtextxy(10, 90, zombie_text);

        // 绘制玩家角色
        if (moving_left || moving_right || moving_up || moving_down) {
            if (facing_left) {
                anim_player_left->showimage(player_position.x, player_position.y, delta);
            }
            else {
                anim_player_right->showimage(player_position.x, player_position.y, delta);
            }
        }
        else {
            if (facing_left) {
                putimage_alpha(player_position.x, player_position.y, atlas_player_left->frame_list[0]);
            }
            else {
                putimage_alpha(player_position.x, player_position.y, atlas_player_right->frame_list[0]);
            }
        }

        if (selected_plant > 0) {
            TCHAR s[20];
            _stprintf_s(s, _T("Selected: %d"), selected_plant);
            outtextxy(10, 10, s);
        }

        FlushBatchDraw();

        // 控制帧率
        DWORD endTime = GetTickCount();
        DWORD delta_time = endTime - startTime;
        if (delta_time < 1000 / 144) {
            Sleep(1000 / 144 - delta_time);
        }
    }

    // 清理资源
    for (auto zombie : zombies) {
        delete zombie;
    }
    zombies.clear();

    for (auto plant : plants) {
        delete plant;
    }
    plants.clear();

    delete brain;
    delete anim_player_left;
    delete anim_player_right;
    delete atlas_player_left;
    delete atlas_player_right;

    closegraph();
    return 0;
}
