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
    ExMessage msg; // 消息结构体 方便之后处理消息

    IMAGE img_background; // 背景图片类
    IMAGE test_img; // 测试图片类
    IMAGE sun_back; // 测试图片类
    IMAGE tombstone; // 墓碑图片类

    // 植物管理
    int selected_plant = 0; // 当前选中的植物类型（0表示未选中）
    std::vector<Plant*> plants; // 存储所有放置的植物

    // 初始化移动控制变量
    bool facing_left = true; // 角色朝向，true为左，false为右
    bool moving_up = false;
    bool moving_down = false;
    bool moving_left = false;
    bool moving_right = false;

    loadimage(&img_background, _T("img/background.png")); // 加载背景图片
    //loadimage(&test_img, _T("img/SunFlower1.gif"));// 加载测试图片
    loadimage(&sun_back, _T("img/sun_back.png")); // 加载阳光栏
    loadimage(&tombstone, _T("img/tombstone.png"), 150, 150); // 加载墓碑 宽度：150 高度：150


    BeginBatchDraw(); // 开启批量绘图
    while (running) {
        DWORD startTime = GetTickCount(); // 节省时间部分

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
        if (player_position.x > 1280 - 80) player_position.x = 1280 - 70; // 假设角色宽度为80
        if (player_position.y > 720 - 80) player_position.y = 720 - 80;   // 假设角色高度为80

        // 更新所有植物

        cleardevice(); // 清空屏幕，然后绘制新帧
//*****************************下方为要绘制上去的内容********************************//

        DWORD delta = 1000 / 144;
        for (auto plant : plants) {
            plant->Update(delta);
        }
        putimage(0, 0, &img_background); //放置背景图片于（0，0）
        //putimage_alpha(0, 0, &test_img);//放置测试图片于（0，0）
        putimage_alpha(0, 0, &sun_back);
        putimage_alpha(1000, 50, &tombstone);
        // 绘制所有植物
        for (auto plant : plants) {
            plant->Draw();
        }

        // 绘制阳光数量
        setfillcolor(RGB(255, 215, 0)); // 设置数字颜色为金色
        settextcolor(RGB(0, 0, 0));     // 设置文字颜色为黑色
        TCHAR sun_text[20];
        _stprintf_s(sun_text, _T("%d"), sun_count);
        RECT text_rect = { 68, 2, 120, 32 }; // 定义文本显示区域
        drawtext(sun_text, &text_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE); // 居中对齐显示

        // 使用Animation类显示角色动画
        if (moving_left || moving_right || moving_up || moving_down) {
            //******************只在移动时更新动画****************//
            if (facing_left) {
                anim_player_left->showimage(player_position.x, player_position.y, delta);
            }
            else {
                anim_player_right->showimage(player_position.x, player_position.y, delta);
            }
        }
        else {
            // 静止时显示第一帧
            if (facing_left) {
                putimage_alpha(player_position.x, player_position.y, atlas_player_left->frame_list[0]);
            }
            else {
                putimage_alpha(player_position.x, player_position.y, atlas_player_right->frame_list[0]);
            }
        }

        // 如果有植物被选中，显示当前选中的植物类型
        if (selected_plant > 0) {
            TCHAR s[20];
            _stprintf_s(s, _T("Selected: %d"), selected_plant);
            outtextxy(10, 10, s);
        }

        FlushBatchDraw(); // 刷新屏幕，显示新绘制的内容

        // 控制帧率
        DWORD endTime = GetTickCount();
        DWORD delta_time = endTime - startTime;
        if (delta_time < 1000 / 144) {
            Sleep(1000 / 144 - delta_time);
        }
    }

    // 清理资源
    for (auto plant : plants) {
        delete plant;
    }
    plants.clear();

    delete anim_player_left;
    delete anim_player_right;
    delete atlas_player_left;
    delete atlas_player_right;


    closegraph(); // 关闭图形窗口
    return 0;
}
