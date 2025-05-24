#include <graphics.h>      // EasyX图形库
#include <string>
#include <iostream>
#include <windows.h>      // Windows API
#include <vector>         // 标准模板库vector容器
#include <cmath>          // 数学函数库
#include <conio.h>         // 控制台输入输出
#include <random>
#include <algorithm>

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
int sun_count = 200;                  // 初始阳光数量
#define WIDTH 1280                   // 窗口宽度
#define HEIGHT 720                   // 窗口高度




// 创建全局的玩家动画图集对象
Atlas* atlas_player_left = new Atlas(_T("img/player_left_%d.png"), 6);   // 左向动画帧
Atlas* atlas_player_right = new Atlas(_T("img/player_right_%d.png"), 6);  // 右向动画帧



// 游戏状态枚举
enum GameState {
    START_SCREEN,
    SELECT_ZOMBIES,
    PLACE_BASE,
    PLAYING,
    PAUSED,
    GAME_OVER
};

// 预定义的tombstone位置
struct TombstonePosition {
    int x, y;
    TombstonePosition(int px, int py) : x(px), y(py) {}
};
// 检查点是否在矩形内
bool isPointInRect(int px, int py, int x, int y, int width, int height) {
    return px >= x && px <= x + width && py >= y && py <= y + height;
}
// 绘制中文文本函数
void drawChineseText(int x, int y, const TCHAR* text, int height = 30, COLORREF color = WHITE) {
    setbkmode(TRANSPARENT);
    LOGFONT font;
    gettextstyle(&font);
    font.lfHeight = height;
    _tcscpy_s(font.lfFaceName, _T("微软雅黑"));
    font.lfQuality = ANTIALIASED_QUALITY;
    font.lfWeight = FW_NORMAL;
    settextstyle(&font);
    settextcolor(color);
    outtextxy(x, y, text);
}

int main() {
    std::random_device rd;  // 硬件熵源（可能慢但不可预测）
    std::mt19937 g(rd());   // 用 rd() 的输出来播种引擎

    initgraph(WIDTH, HEIGHT); // 初始化图形窗口，设置大小为1280x720
    cleardevice();

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

    // 加载资源
    Atlas* backgroundAtlas = nullptr;
    Atlas* beginButtonAtlas = nullptr;
    Atlas* button1Atlas = nullptr;
    Atlas* button2Atlas = nullptr;
    Atlas* button3Atlas = nullptr;
    Atlas* tombstoneAtlas = nullptr;
    Atlas* brainBaseAtlas = nullptr;
	Atlas* pauseButtonAtlas = nullptr;

    backgroundAtlas = new Atlas(_T("img/background.png"));// 背景
    beginButtonAtlas = new Atlas(_T("img/begin_idle.png"));// 开始按键
    button1Atlas = new Atlas(_T("img/botton_1.png"));// 选择墓碑数量1按键
    button2Atlas = new Atlas(_T("img/botton_2.png"));// 选择墓碑数量2按键
    button3Atlas = new Atlas(_T("img/botton_3.png"));// 选择墓碑数量3按键
    tombstoneAtlas = new Atlas(_T("img/tombstone.png"));// 墓碑
    brainBaseAtlas = new Atlas(_T("img/brain_base.png"));// 大脑基地
    pauseButtonAtlas = new Atlas(_T("img/pause_idle.png"));// 暂停按键
    ExMessage msg;              // 消息结构体，用于处理用户输入



    //// 加载游戏资源
    //IMAGE img_background;        // 背景图片
    //IMAGE sun_back;             // 阳光计数器背景
    //IMAGE tombstone;            // 墓碑图片



    // 初始化游戏状态和变量
    GameState gameState = START_SCREEN;
    int lastTime = GetTickCount();
    bool running = true;
    int selectedZombieCount = 0;
    std::vector<TombstonePosition> tombstonePositions;
    TombstonePosition basePosition(-1, -1);
    // 预定义的tombstone生成位置
    std::vector<TombstonePosition> possibleTombstonePositions = {
        TombstonePosition(100, 100),
        TombstonePosition(300, 200),
        TombstonePosition(500, 150),
        TombstonePosition(700, 300),
        TombstonePosition(900, 100),
        TombstonePosition(1100, 250),
        TombstonePosition(200, 400),
        TombstonePosition(600, 450),
        TombstonePosition(1000, 400)
    };
    




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

    //// 加载图片资源
    //loadimage(&img_background, _T("img/background.png"));            // 加载背景
    //loadimage(&sun_back, _T("img/sun_back.png"));                   // 加载阳光栏
    //loadimage(&tombstone, _T("img/tombstone.png"), 150, 150);       // 加载墓碑

    // 初始化僵尸系统
    POINT tombstone_pos = { 1000, 50 };        // 设置墓碑（僵尸生成点）位置
    ZombieSpawner spawner(tombstone_pos);      // 创建僵尸生成器
    std::vector<Zombie*> zombies;              // 僵尸容器

    

    //// 游戏开始前的大脑放置阶段
    //settextcolor(WHITE);
    //settextstyle(40, 0, _T("Arial"));
    //outtextxy(400, 300, _T("点击屏幕放置大脑基地"));
    //FlushBatchDraw();








    //// 等待玩家放置大脑
    //while (!game_started && running) {
    //    if (peekmessage(&msg)) {
    //        if (msg.message == WM_LBUTTONDOWN) {
    //            POINT click_pos = { msg.x, msg.y };
    //            // 确保大脑放置在合理位置
    //            if (click_pos.x > 100 && click_pos.x < 1180 && 
    //                click_pos.y > 100 && click_pos.y < 620) {
    //                brain->SetPosition(click_pos);
    //                game_started = true;
    //            }
    //        }
    //        else if (msg.message == WM_KEYDOWN && msg.vkcode == VK_ESCAPE) {
    //            running = false;
    //        }
    //    }
    //}

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

        // 处理用户输入——阶段处理部分
        while (peekmessage(&msg)) {
            switch (gameState) {
            case START_SCREEN: {
                // 开始界面输入处理
                if (msg.message == WM_LBUTTONDOWN) {
                    // 检查是否点击了开始游戏按钮 
                    // 次数仅判断点击，之后统一绘图
                    int buttonX = WIDTH / 2 - 100;
                    int buttonY = HEIGHT / 2 + 50;
                    if (isPointInRect(msg.x, msg.y, buttonX, buttonY, 250, 250)) {
                        gameState = SELECT_ZOMBIES;
                    }
                }
                break;
            }
            case SELECT_ZOMBIES: {
                // 选择僵尸数量界面
                if (msg.message == WM_LBUTTONDOWN) {
                    int button1X = WIDTH / 2 - 375; // 三个按钮水平排列
                    int button2X = WIDTH / 2 - 125;
                    int button3X = WIDTH / 2 + 125;
                    int buttonY = HEIGHT / 2 - 125;

                    if (isPointInRect(msg.x, msg.y, button1X, buttonY, 250, 250)) {
                        selectedZombieCount = 1;
                    }
                    else if (isPointInRect(msg.x, msg.y, button2X, buttonY, 250, 250)) {
                        selectedZombieCount = 2;
                    }
                    else if (isPointInRect(msg.x, msg.y, button3X, buttonY, 250, 250)) {
                        selectedZombieCount = 3;
                    }

                    if (selectedZombieCount > 0) {
                        tombstonePositions.clear();  // 清空之前的墓碑位置
                        std::vector<int> indices;
                        for (int i = 0; i < possibleTombstonePositions.size(); i++) {
                            indices.push_back(i);  // 生成索引列表 [0, 1, 2, ..., N-1]
                        }
                        std::shuffle(indices.begin(), indices.end(),g);  // 随机打乱索引

                        // 根据选择的僵尸数量，从打乱的索引中选取对应数量的位置
                        for (int i = 0; i < selectedZombieCount; i++) {
                            tombstonePositions.push_back(possibleTombstonePositions[indices[i]]);
                        }
                        gameState = PLACE_BASE;  // 进入放置基地状态
                    }
                }
                break;
            }
            case PLACE_BASE: {
                // 选择基地位置
                if (msg.message == WM_LBUTTONDOWN) {
                    // 检查点击位置是否与tombstone重叠
                    bool validPosition = true;
                    for (const auto& tomb : tombstonePositions) {
                        // 检查点击位置是否与tombstone重叠
                        if (abs(msg.x - tomb.x) < 200 && abs(msg.y - tomb.y) < 200) {  // 避免重叠
                            validPosition = false;
                            break;
                        }
                    }

                    // 检查是否在屏幕边界内
                    if (msg.x < 125 || msg.x > WIDTH - 125 || msg.y < 125 || msg.y > HEIGHT - 125) {
                        validPosition = false;
                    }

                    if (validPosition) {
                        basePosition.x = msg.x - 125; // 居中放置
                        basePosition.y = msg.y - 125;
                        gameState = PLAYING;
                    }
                }
                break;
            }
            case PLAYING: {
                // 游戏中输入处理
                //由于这里机制复杂，因此相关逻辑放到阶段选择之后进行书写
				//这里仅处理暂停
                if (msg.message == WM_KEYDOWN && msg.vkcode == VK_ESCAPE) {
                    gameState = PAUSED;
                }
                break;
            }
            case PAUSED: {
                // 暂停状态输入处理
                if (msg.message == WM_KEYDOWN && msg.vkcode == VK_ESCAPE) {
                    gameState = GAME_OVER;
                }
                else if (msg.message == WM_LBUTTONDOWN) {
                    gameState = PLAYING;
                }
                break;
            }
            case GAME_OVER: {
                running = false;
                break;
            }
            }
        }

		//***************************************阶段控制结束，开始后续按键处理*****************************************//

        if(gameState == PLAYING){
            switch (msg.message) {
                case WM_KEYDOWN:    // 按键按下事件
                    switch (msg.vkcode) {
                        case '1': selected_plant = 1; break;  // 选择向日葵
                        case '2': selected_plant = 2; break;  // 选择豌豆射手
                        case '3': selected_plant = 3; break;  // 选择坚果墙
                        case 'w': case VK_UP: moving_up = true; break;
                        case 's': case VK_DOWN: moving_down = true; break;
                        case 'a': case VK_LEFT: 
                            moving_left = true; 
                            facing_left = true; 
                            break;
                        case 'D': case VK_RIGHT: 
                            moving_right = true; 
                            facing_left = false; 
                            break;
                        //case VK_ESCAPE: running = false; break;
                    }
                    break;

                case WM_KEYUP:      // 按键释放事件
                    switch (msg.vkcode) {
                        case '1': case '2': case '3': selected_plant = 0; break;
                        case 'w': case VK_UP: moving_up = false; break;
                        case 's': case VK_DOWN: moving_down = false; break;
                        case 'a': case VK_LEFT: moving_left = false; break;
                        case 'd': case VK_RIGHT: moving_right = false; break;
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
		// *********************************按键处理结束****************************************//
        BeginBatchDraw(); // 开始批量绘图，防止闪烁
        // 更新游戏状态
        DWORD delta = 1000 / 144;  // 计算帧间隔时间

        // 更新玩家位置
        //此处需要改进计数器系统进行优化，否则会导致玩家移动忽快忽慢
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
        // *********************************开始绘图****************************************//
        switch (gameState) {
        case START_SCREEN: {
            // 绘制开始界面
            cleardevice();
            // 显示背景
            putimage_alpha(0, 0, backgroundAtlas->frame_list[0]);

            // 显示开始游戏按钮（使用图片）
            int buttonX = WIDTH / 2 - 100;
            int buttonY = HEIGHT / 2 + 50;
            putimage_alpha(buttonX, buttonY, beginButtonAtlas->frame_list[0]);
            break;
        }
        case SELECT_ZOMBIES: {
            // 绘制僵尸数量选择界面
            cleardevice();
            putimage_alpha(0, 0, backgroundAtlas->frame_list[0]);

            // 显示提示文字
            drawChineseText(WIDTH / 2 - 200, HEIGHT / 2 - 250, _T("选择僵尸生成点数量"), 40, RGB(255, 255, 255));
            
            // 显示三个按钮
            int button1X = WIDTH / 2 - 375;
            int button2X = WIDTH / 2 - 125;
            int button3X = WIDTH / 2 + 125;
            int buttonY = HEIGHT / 2 - 125;

            putimage_alpha(button1X, buttonY, button1Atlas->frame_list[0]);
            putimage_alpha(button2X, buttonY, button2Atlas->frame_list[0]);
            putimage_alpha(button3X, buttonY, button3Atlas->frame_list[0]);
            break;
        }
        case PLACE_BASE: {
            // 绘制基地放置界面
            cleardevice();
            putimage_alpha(0, 0, backgroundAtlas->frame_list[0]);

            // 显示已选择的tombstone
            for (const auto& tomb : tombstonePositions) {
                putimage_alpha(tomb.x, tomb.y, tombstoneAtlas->frame_list[0]);
            }

            // 显示提示文字
            drawChineseText(WIDTH / 2 - 150, 50, _T("点击选择基地位置"), 40, RGB(255, 255, 255));

            // 显示鼠标位置的基地预览（半透明效果  可以通过调整alpha实现）
            //ExMessage mouseMsg;
            //if (peekmessage(&mouseMsg, EX_MOUSE)) {
            //简单预览，显示基地可能的位置
            // 暂时不加，之后改进
            //    drawChineseText(mouseMsg.x - 50, mouseMsg.y - 30, _T("基地"), 20, RGB(0, 255, 0));
            //}
            break;
        }
        case PLAYING: {
            // 绘制游戏界面
            cleardevice();
            putimage_alpha(0, 0, backgroundAtlas->frame_list[0]);

            // 显示tombstone
            for (const auto& tomb : tombstonePositions) {
                putimage_alpha(tomb.x, tomb.y, tombstoneAtlas->frame_list[0]);
            }

            // 显示基地
            if (basePosition.x >= 0 && basePosition.y >= 0) {
                putimage_alpha(basePosition.x, basePosition.y, brainBaseAtlas->frame_list[0]);
            }

            // 在这里可以添加更多游戏内容
            drawChineseText(10, 10, _T("游戏进行中 - 按ESC暂停"), 20, RGB(255, 255, 255));
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
            
            
            
            
            
            
            
            
            
            break;
        }
        case PAUSED: {
            // 绘制暂停界面
            cleardevice();
            putimage_alpha(0, 0, backgroundAtlas->frame_list[0]);
            // 显示开始游戏按钮（使用图片）
            int buttonX = WIDTH / 2 - 100;
            int buttonY = HEIGHT / 2 + 50;
            putimage_alpha(buttonX, buttonY, pauseButtonAtlas->frame_list[0]);
            // 显示游戏内容
            for (const auto& tomb : tombstonePositions) {
                putimage_alpha(tomb.x, tomb.y, tombstoneAtlas->frame_list[0]);
            }
            if (basePosition.x >= 0 && basePosition.y >= 0) {
                putimage_alpha(basePosition.x, basePosition.y, brainBaseAtlas->frame_list[0]);
            }

            // 暂停信息
            //putimage_alpha(WIDTH / 2 - 200, HEIGHT / 2 - 50, pauseText);
            //drawChineseText(WIDTH / 2 - 180, HEIGHT / 2 - 30, _T("游戏暂停 - 点击继续"), 40);
            break;
        }
        case GAME_OVER: {
            break;
            }


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