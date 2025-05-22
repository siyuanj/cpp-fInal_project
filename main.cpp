#include <graphics.h>      // EasyX图形库
#include <string>
#include <iostream>
#include <windows.h>      // Windows API
#include <vector>         // 标准模板库vector容器
#include <cmath>          // 数学函数库

// 包含自定义类的头文件
#include "Animation.h"        // 动画系统
#include "Plant.h"           // 植物基类
#include "SpecificPlants.h"  // 具体植物类
#include "Bullet.h"          // 子弹系统
#include "Sun.h"             // 阳光系统
#include "Zombie.h"          // 僵尸系统
#include "BrainBase.h"       // 大脑基地类

// 链接Windows图像处理库，用于支持透明图片
#pragma comment(lib, "MSIMG32.LIB")

// 全局常量和变量
const double PI = 3.1415926;         // 圆周率常量
double PLAYER_SPEED = 5.0;           // 玩家移动速度
POINT player_position = { 500,500 }; // 玩家初始位置
int sun_count = 50;                  // 初始阳光数量

// 创建全局的玩家动画图集对象
Atlas* atlas_player_left = new Atlas(_T("img/player_left_%d.png"), 6);   // 左向动画帧
Atlas* atlas_player_right = new Atlas(_T("img/player_right_%d.png"), 6);  // 右向动画帧

int main() {
    initgraph(1280, 720); // 初始化图形窗口，设置大小为1280x720

    // 设置文字显示样式
    LOGFONT font;
    gettextstyle(&font);
    font.lfHeight = 30;                         // 字体高度
    _tcscpy_s(font.lfFaceName, _T("Arial"));   // 字体类型
    font.lfQuality = ANTIALIASED_QUALITY;       // 抗锯齿
    settextstyle(&font);                        // 应用字体设置
    setbkmode(TRANSPARENT);                     // 文字背景透明

    // 创建玩家动画对象
    Animation* anim_player_left = new Animation(atlas_player_left, 45);   // 左向动画
    Animation* anim_player_right = new Animation(atlas_player_right, 45); // 右向动画

    // 游戏状态标志
    bool running = true;         // 游戏运行标志
    bool game_started = false;   // 游戏是否开始（是否已放置大脑）
    ExMessage msg;              // 消息结构体，用于处理用户输入

    // 加载游戏资源
    IMAGE img_background;        // 背景图片
    IMAGE sun_back;             // 阳光计数器背景
    IMAGE tombstone;            // 墓碑图片

    // 创建游戏对象
    BrainBase* brain = new BrainBase();    // 创建大脑基地
    std::vector<Plant*> plants;            // 植物容器
    int selected_plant = 0;                // 当前选中的植物类型（0表示未选中）

    // 玩家移动控制变量
    bool facing_left = true;    // 角色朝向（true为左，false为右）
    bool moving_up = false;     // 向上移动标志
    bool moving_down = false;   // 向下移动标志
    bool moving_left = false;   // 向左移动标志
    bool moving_right = false;  // 向右移动标志

    // 加载图片资源
    loadimage(&img_background, _T("img/background.png"));            // 加载背景
    loadimage(&sun_back, _T("img/sun_back.png"));                   // 加载阳光栏
    loadimage(&tombstone, _T("img/tombstone.png"), 150, 150);       // 加载墓碑

    // 初始化僵尸系统
    POINT tombstone_pos = { 1000, 50 };        // 设置墓碑（僵尸生成点）位置
    ZombieSpawner spawner(tombstone_pos);      // 创建僵尸生成器
    std::vector<Zombie*> zombies;              // 僵尸容器

    BeginBatchDraw(); // 开始批量绘图，防止闪烁

    // 游戏开始前的大脑放置阶段
    settextcolor(WHITE);
    settextstyle(40, 0, _T("Arial"));
    outtextxy(400, 300, _T("点击屏幕放置大脑基地"));
    FlushBatchDraw();

    // 等待玩家放置大脑
    while (!game_started && running) {
        if (peekmessage(&msg)) {
            if (msg.message == WM_LBUTTONDOWN) {
                POINT click_pos = { msg.x, msg.y };
                // 确保大脑放置在合理位置
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
        DWORD startTime = GetTickCount(); // 记录帧开始时间

        // 检查游戏是否失败
        if (!brain->IsAlive()) {
            cleardevice();
            settextcolor(RED);
            settextstyle(60, 0, _T("Arial"));
            outtextxy(480, 300, _T("游戏结束!"));
            FlushBatchDraw();
            Sleep(2000);
            running = false;
            continue;
        }

        // 处理用户输入
        while (peekmessage(&msg)) {
            switch (msg.message) {
                case WM_KEYDOWN:    // 按键按下事件
                    switch (msg.vkcode) {
                        case '1': selected_plant = 1; break;  // 选择向日葵
                        case '2': selected_plant = 2; break;  // 选择豌豆射手
                        case '3': selected_plant = 3; break;  // 选择坚果墙
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

                case WM_KEYUP:      // 按键释放事件
                    switch (msg.vkcode) {
                        case '1': case '2': case '3': selected_plant = 0; break;
                        case 'W': case VK_UP: moving_up = false; break;
                        case 'S': case VK_DOWN: moving_down = false; break;
                        case 'A': case VK_LEFT: moving_left = false; break;
                        case 'D': case VK_RIGHT: moving_right = false; break;
                    }
                    break;

                case WM_LBUTTONDOWN:    // 鼠标左键点击事件
                    if (selected_plant > 0) {
                        POINT click_pos = { msg.x, msg.y };
                        Plant* new_plant = nullptr;
                        int cost = 0;

                        // 根据选择创建对应的植物
                        switch (selected_plant) {
                            case 1: // 向日葵
                                cost = 50;
                                if (sun_count >= cost) {
                                    new_plant = new Sunflower(click_pos);
                                    sun_count -= cost;
                                }
                                break;
                            case 2: // 豌豆射手
                                cost = 100;
                                if (sun_count >= cost) {
                                    new_plant = new Peashooter(click_pos);
                                    sun_count -= cost;
                                }
                                break;
                            case 3: // 坚果墙
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

        // 更新游戏状态
        DWORD delta = 1000 / 144;  // 计算帧间隔时间

        // 更新玩家位置
        if (moving_up) player_position.y -= PLAYER_SPEED;
        if (moving_down) player_position.y += PLAYER_SPEED;
        if (moving_left) player_position.x -= PLAYER_SPEED;
        if (moving_right) player_position.x += PLAYER_SPEED;

        // 玩家边界检测
        if (player_position.x < 0) player_position.x = 0;
        if (player_position.y < 0) player_position.y = 0;
        if (player_position.x > 1280 - 80) player_position.x = 1280 - 81;
        if (player_position.y > 720 - 80) player_position.y = 720 - 81;

        // 更新僵尸系统
        if (Zombie* new_zombie = spawner.Update(delta)) {
            zombies.push_back(new_zombie);
        }

        // 更新所有僵尸
        for (auto zombie : zombies) {
            zombie->Update(delta, plants, brain);
        }

        // 清理死亡的僵尸
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

        // 更新植物
        for (auto plant : plants) {
            plant->Update(delta);
        }

        // 绘制游戏画面
        cleardevice();  // 清空屏幕

        // 绘制背景层
        putimage(0, 0, &img_background);
        putimage_alpha(0, 0, &sun_back);
        putimage_alpha(1000, 50, &tombstone);

        // 绘制游戏对象
        brain->Draw();              // 绘制大脑基地
        for (auto plant : plants) { // 绘制植物
            plant->Draw();
        }
        for (auto zombie : zombies) { // 绘制僵尸
            zombie->Draw();
        }

        // 绘制UI层
        // 阳光数量
        setfillcolor(RGB(255, 215, 0));
        settextcolor(RGB(0, 0, 0));
        TCHAR sun_text[20];
        _stprintf_s(sun_text, _T("%d"), sun_count);
        RECT text_rect = { 68, 2, 120, 32 };
        drawtext(sun_text, &text_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        // 大脑血量
        TCHAR hp_text[20];
        _stprintf_s(hp_text, _T("大脑血量: %d"), brain->GetHP());
        outtextxy(10, 50, hp_text);

        // 僵尸数量（调试信息）
        TCHAR zombie_text[20];
        _stprintf_s(zombie_text, _T("僵尸数量: %d"), (int)zombies.size());
        outtextxy(10, 90, zombie_text);

        // 绘制玩家角色
        if (moving_left || moving_right || moving_up || moving_down) {
            // 移动时播放动画
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

        // 显示当前选中的植物
        if (selected_plant > 0) {
            TCHAR s[20];
            _stprintf_s(s, _T("Selected: %d"), selected_plant);
            outtextxy(10, 10, s);
        }

        FlushBatchDraw(); // 更新屏幕显示

        // 控制帧率
        DWORD endTime = GetTickCount();
        DWORD delta_time = endTime - startTime;
        if (delta_time < 1000 / 144) {
            Sleep(1000 / 144 - delta_time);
        }
    }

    // 清理资源
    // 清理僵尸
    for (auto zombie : zombies) {
        delete zombie;
    }
    zombies.clear();

    // 清理植物
    for (auto plant : plants) {
        delete plant;
    }
    plants.clear();

    // 清理其他资源
    delete brain;
    delete anim_player_left;
    delete anim_player_right;
    delete atlas_player_left;
    delete atlas_player_right;

    closegraph(); // 关闭图形窗口
    return 0;
}
