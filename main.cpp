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
#include "tombstone.h"       // 墓碑类

// 链接Windows图像处理库，用于支持透明图片
#pragma comment(lib, "MSIMG32.LIB")


// 全局常量和变量
const double PI = 3.1415926;         // 圆周率常量
double PLAYER_SPEED = 5.0;           // 玩家移动速度
POINT player_position = { 500,500 }; // 玩家初始位置
int sun_count = 500;                  // 初始阳光数量
#define WIDTH 1280                   // 窗口宽度
#define HEIGHT 720                   // 窗口高度

// 创建全局的玩家动画图集对象
Atlas* atlas_player_left;
Atlas* atlas_player_right;

// 预定义的tombstone位置
struct TombstonePosition {
    int x, y;
    TombstonePosition(int px, int py) : x(px), y(py) {}
};

// 游戏状态枚举
enum GameState {
    START_SCREEN,
    SELECT_ZOMBIES,
    PLACE_BASE,
    PLAYING,
    PAUSED,
    GAME_OVER,
    GAME_VICTORY, // 游戏胜利状态
    UPGRADE_CHOICE // 回合结束强化选择状态
};

// 检查点是否在矩形内
static bool isPointInRect(int px, int py, int x, int y, int width, int height) {
    return px >= x && px <= x + width && py >= y && py <= y + height;
}
// 绘制中文文本函数
static void drawChineseText(int x, int y, const TCHAR* text, int height = 30, COLORREF color = WHITE) {
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

    // 初始化全局 Atlas 对象
    atlas_player_left = new Atlas(_T("img/player_left_%d.png"), 6);   // 左向动画帧
    atlas_player_right = new Atlas(_T("img/player_right_%d.png"), 6);  // 右向动画帧

    // 创建玩家动画对象
    Animation* anim_player_left = new Animation(atlas_player_left, 45);   // 左向动画
    Animation* anim_player_right = new Animation(atlas_player_right, 45); // 右向动画

    // 加载资源
    Atlas* backgroundAtlas = nullptr;
    Atlas* beginButtonAtlas = nullptr;
    Atlas* button1Atlas = nullptr;
    Atlas* button2Atlas = nullptr;
    Atlas* button3Atlas = nullptr;
    Atlas* brainBaseAtlas = nullptr;
    Atlas* pauseButtonAtlas = nullptr;
    Atlas* sun_back = nullptr;
    Atlas* seedbank = nullptr;
    Atlas* gameover_botton = nullptr;
    Atlas* victory_botton = nullptr;

    POINT tombstone_pos = { 1000, 50 }; // 初始墓碑位置

    backgroundAtlas = new Atlas(_T("img/background.png"));// 背景
    beginButtonAtlas = new Atlas(_T("img/begin_idle.png"));// 开始按键
    button1Atlas = new Atlas(_T("img/botton_1.png"));// 选择墓碑数量1按键
    button2Atlas = new Atlas(_T("img/botton_2.png"));// 选择墓碑数量2按键
    button3Atlas = new Atlas(_T("img/botton_3.png"));// 选择墓碑数量3按键
    brainBaseAtlas = new Atlas(_T("img/brain_base.png"));// 大脑基地
    pauseButtonAtlas = new Atlas(_T("img/pause_idle.png"));// 暂停按键
    //sun_back = new Atlas(_T("img/sun_back.png"));// 阳光栏
    seedbank = new Atlas(_T("img/seedbank_plant.png"));// 种子银行
    gameover_botton = new Atlas(_T("img/gameover_eng.png"));// 结束按键
    victory_botton = new Atlas(_T("img/victory_botton.png"));// 结束按键


    ExMessage msg;              // 消息结构体，用于处理用户输入

    // 初始化游戏状态和变量
    GameState gameState = START_SCREEN;
    int lastTime = GetTickCount();
    bool running = true;

    // tombstone 管理
    int selectedZombieCount = 0;// 选择的墓碑数量
	std::vector<tombstone*> tombstones; // 存储tombstone指针的数组，针对tombstone类的指针数组
    POINT basePosition = {-1, -1};// 基地位置，初始为无效值
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

    // ---- 回合制机制变量 START ----
    int currentRound = 0;
    const int MAX_ROUNDS = 5;
    int zombiesToSpawnThisRound = 0;
    int zombiesSpawnedThisRound = 0;
    const float INITIAL_ZOMBIE_SPAWN_INTERVAL = 3000.0f; // 初始僵尸生成间隔 (毫秒)
    float currentZombieSpawnInterval = INITIAL_ZOMBIE_SPAWN_INTERVAL;
    std::vector<POINT> current_tombstone_positions_for_spawner; // 用于存储传递给 Spawner 的墓碑位置

    bool isResting = false; // 是否处于回合间歇期
    DWORD roundOverTime = 0; // 回合结束时间点
    const DWORD REST_PERIOD = 10000; // 休息时间 10秒 (毫秒)

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

    // 初始化僵尸系统
    ZombieSpawner* spawner = nullptr; // 声明spawner指针
    std::vector<Zombie*> zombies;              // 僵尸容器
    // 全局子弹容器
    std::vector<Bullet*> bullets; // 用于存储所有在场子弹

    // 主游戏循环
    while (running) {
        DWORD startTime = GetTickCount(); // 记录帧开始时间

        // 检查游戏是否失败
        if (!brain->IsAlive()) {
            cleardevice();
			gameState = GAME_OVER; // 设置游戏状态为结束
            
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
                        // 清理旧的tombstone对象
                        for (auto t : tombstones) {
                            delete t;
                        }
                        tombstones.clear();  // 清空之前的墓碑
                        std::vector<int> indices;
                        for (int i = 0; i < possibleTombstonePositions.size(); i++) {
                            indices.push_back(i);  // 生成索引列表 [0, 1, 2, ..., N-1]
                        }
                        std::shuffle(indices.begin(), indices.end(), g);  // 随机打乱索引

                        for (int i = 0; i < selectedZombieCount && i < indices.size(); i++) {
                            POINT p = { possibleTombstonePositions[indices[i]].x, possibleTombstonePositions[indices[i]].y };
                            tombstones.push_back(new tombstone(p));
                            current_tombstone_positions_for_spawner.push_back(p); // 正确收集墓碑位置
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
                    for (const auto& tomb : tombstones) { // 修改为遍历tombstone对象
                        POINT tombPos = tomb->getPosition(); // 获取tombstone位置
                        // 检查点击位置是否与tombstone重叠
                        if (abs(msg.x - tombPos.x) < 200 && abs(msg.y - tombPos.y) < 200) {  // 避免重叠
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
                        brain->SetPosition(basePosition); // **新增：设置BrainBase的实际位置**
                        gameState = PLAYING;
                    }
                    // ---- 开始第一回合逻辑 START ----
                    currentRound = 1; // 直接设为第一回合
                    zombiesToSpawnThisRound = 5 + 10 * currentRound;
                    zombiesSpawnedThisRound = 0;// 重置已生成的僵尸数量
                    // 计算本回合僵尸生成间隔（每回合减少20%）
                    currentZombieSpawnInterval = INITIAL_ZOMBIE_SPAWN_INTERVAL * static_cast<float>(pow(0.8, currentRound - 1));

                    if (spawner) {
                        delete spawner;
                        spawner = nullptr;
                    }
                    if (!current_tombstone_positions_for_spawner.empty()) {
                        spawner = new ZombieSpawner(current_tombstone_positions_for_spawner, static_cast<int>(currentZombieSpawnInterval), 1.0f);
                        // 1.0f 表示100%生成概率
						//每回合重新创建spawner对象，用于修改间隔
                    }
                    else {
                        // 处理没有墓碑位置的情况，理论上不应发生
                        drawChineseText(WIDTH / 2 - 100, HEIGHT / 2, _T("错误：没有墓碑位置！"), 30, RED);
                        FlushBatchDraw(); Sleep(3000); running = false;
                    }
                    isResting = false;
                    // ---- 开始第一回合逻辑 END ----
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
                switch (msg.message) {
                case WM_KEYDOWN:    // 按键按下事件
                    switch (msg.vkcode) {
                    case '1': selected_plant = 1; break;  // 选择向日葵
                    case '2': selected_plant = 2; break;  // 选择豌豆射手
                    case '3': selected_plant = 3; break;  // 选择坚果墙
                    case '4': selected_plant = 4; break;  // 选择高坚果
                    case '5': selected_plant = 5; break;  // 选择双发射手
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
                        //case VK_ESCAPE: running = false; break;
                    }
                    break;

                case WM_KEYUP:      // 按键释放事件
                    switch (msg.vkcode) {
                    case '1': case '2': case '3': case '4': case '5':selected_plant = 0; break;
                    case 'W': case VK_UP: moving_up = false; break;
                    case 'S': case VK_DOWN: moving_down = false; break;
                    case 'A': case VK_LEFT: moving_left = false; break;
                    case 'D': case VK_RIGHT: moving_right = false; break;
                    }
                    break;

                case WM_LBUTTONDOWN:    // 鼠标左键点击事件
                    if (selected_plant > 0) {
                        POINT click_pos = { msg.x, msg.y };
                        Plant* new_plant = nullptr; //new_plant为指针
                        int cost = 0;

                        // 根据选择创建对应的植物
                        switch (selected_plant) {
                        case 1: // 向日葵
                            cost = 50;
                            if (sun_count >= cost) {
                                new_plant = new Sunflower(click_pos);// 使指针指向新建的植物对象
                                // new关键字的作用是动态分配内存并构造对象，使其不会在函数结束时被销毁
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
                        case 4: // 高坚果
                            cost = 125; // 假设高坚果的阳光消耗为125，你可以根据需要修改
                            if (sun_count >= cost) {
                                new_plant = new TallWallNut(click_pos);
                                sun_count -= cost;
                            }
                            break;
                        case 5: // Repeater
                            cost = 200; // Repeater 的阳光消耗
                            if (sun_count >= cost) {
                                new_plant = new Repeater(click_pos);
                                sun_count -= cost;
                            }
                            break;
                        }


                        if (new_plant) {
                            plants.push_back(new_plant);// 新指针存入植物数组中
                        }
                    }
                    break;
                }
                break;
            }
            case PAUSED: {
                // 暂停状态输入处理
                if (msg.message == WM_KEYDOWN && msg.vkcode == VK_ESCAPE) {
                    running = false;
                }
                else if (msg.message == WM_LBUTTONDOWN) {
                    gameState = PLAYING;
                }
                break;
            }
                       // 新增：处理强化选择界面的输入
            case UPGRADE_CHOICE: {
                if (msg.message == WM_LBUTTONDOWN) {
                    // 定义强化选项按钮的布局参数
                    int button_width = 600; // 按钮宽度
                    int button_height = 50;  // 按钮高度
                    int button_spacing = 20; // 按钮间距
                    int button_x = WIDTH / 2 - button_width / 2; // 按钮X坐标 (居中)
                    // 按钮Y坐标，从屏幕中间向上排列
                    int button1_y = HEIGHT / 2 - button_height * 2;
                    int button2_y = button1_y + button_height + button_spacing;
                    int button3_y = button2_y + button_height + button_spacing;

                    bool choice_made = false; // 标记是否已做出选择

                    // 检查是否点击了第一个强化选项 (攻击强化)
                    if (isPointInRect(msg.x, msg.y, button_x, button1_y, button_width, button_height)) {
                        // 遍历所有植物，对 Peashooter 和 Repeater 增加攻击力
                        for (Plant* p : plants) {
                            // 假设 Peashooter 和 Repeater 继承自 AttackPlant, 
                            // 并且 AttackPlant 有 IncreaseAttackPower 方法
                            if (dynamic_cast<Peashooter*>(p) || dynamic_cast<Repeater*>(p)) {
                                if (AttackPlant* ap = dynamic_cast<AttackPlant*>(p)) {
                                    ap->IncreaseAttackPower(10); // 攻击力+10
                                }
                            }
                        }
                        choice_made = true;
                    }
                    // 检查是否点击了第二个强化选项 (防御强化)
                    else if (isPointInRect(msg.x, msg.y, button_x, button2_y, button_width, button_height)) {
                        // 遍历所有植物，对 WallNut 和 TallWallNut 增加生命值
                        for (Plant* p : plants) {
                            // 假设 Plant 基类有 IncreaseMaxHPAndHeal 方法
                            if (WallNut* wn = dynamic_cast<WallNut*>(p)) {
                                wn->IncreaseMaxHPAndHeal(100); // 生命值+100
                            }
                            else if (TallWallNut* twn = dynamic_cast<TallWallNut*>(p)) {
                                twn->IncreaseMaxHPAndHeal(100); // 生命值+100
                            }
                        }
                        choice_made = true;
                    }
                    // 检查是否点击了第三个强化选项 (基地强化)
                    else if (isPointInRect(msg.x, msg.y, button_x, button3_y, button_width, button_height)) {
                        // 增加大脑基地生命值
                        // 假设 BrainBase 类有 ApplyHealthUpgrade 方法
                        brain->ApplyHealthUpgrade(300); // 基地生命值+300
                        choice_made = true;
                    }

                    // 如果做出了选择，则进入下一回合的准备阶段
                    if (choice_made) {
                        currentRound++; // 回合数增加

                        // 为下一回合设置参数
                        zombiesToSpawnThisRound = 5 + 5 * currentRound; // 计算下一回合僵尸数量
                        zombiesSpawnedThisRound = 0; // 重置已生成僵尸计数
                        // 计算下一回合僵尸生成间隔
                        currentZombieSpawnInterval = INITIAL_ZOMBIE_SPAWN_INTERVAL * static_cast<float>(pow(0.8, currentRound - 1));

                        // 清理并重新创建僵尸生成器
                        if (spawner) { delete spawner; spawner = nullptr; }
                        if (!current_tombstone_positions_for_spawner.empty()) {
                            spawner = new ZombieSpawner(current_tombstone_positions_for_spawner, static_cast<int>(currentZombieSpawnInterval), 1.0f);
                        }
                        else {
                            // 错误处理：如果没有墓碑位置，游戏可能无法继续
                            drawChineseText(WIDTH / 2 - 100, HEIGHT / 2, _T("错误：没有墓碑位置！"), 30, RED);
                            FlushBatchDraw(); Sleep(3000); running = false;
                        }
                        isResting = true; // 进入回合间歇期
                        roundOverTime = GetTickCount(); // 记录回合结束（强化选择完毕）的时间，用于休息期计时
                        gameState = PLAYING; // 返回游戏进行状态
                    }
                }
                break;
            } // 结束 UPGRADE_CHOICE 输入处理
            case GAME_OVER: {
                
                if (msg.message == WM_LBUTTONDOWN) {
                    running = false;
                    // 点击后退出游戏循环
                }
                //continue;
                break;
            }
            case GAME_VICTORY: {
                if (msg.message == WM_LBUTTONDOWN) running = false;
                break;
            }
            }
        }

        // *********************************按键处理结束****************************************//
        // *********************************更新对象状态****************************************//
        BeginBatchDraw(); // 开始批量绘图，防止闪烁
        // 更新游戏状态
        DWORD delta = 1000 / 144;  // 计算帧间隔时间
        //此处需要改进计数器系统进行优化，否则会导致玩家移动忽快忽慢

        // 更新玩家位置

        if(gameState == PLAYING){
            if (moving_up) player_position.y -= PLAYER_SPEED;
            if (moving_down) player_position.y += PLAYER_SPEED;
            if (moving_left) player_position.x -= PLAYER_SPEED;
            if (moving_right) player_position.x += PLAYER_SPEED;

            // 玩家边界检测
            if (player_position.x < 0) player_position.x = 0;
            if (player_position.y < 0) player_position.y = 0;
            if (player_position.x > 1280 - 80) player_position.x = 1280 - 81;
            if (player_position.y > 720 - 80) player_position.y = 720 - 81;

            //// 更新僵尸系统
            //if (spawner != nullptr) { // 确保 spawner 已被创建
            //    if (Zombie* new_zombie = spawner->Update(delta)) {
            //        // 通过delta判断是否应该生成新的僵尸,Update函数返回一个新的僵尸对象指针
            //        // 僵尸类型、位置等均由Update函数内部逻辑决定
            //        zombies.push_back(new_zombie);
            //    }
            //}


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
            );// ***********************相关语法？？？

            //更新所有僵尸
            for (auto zombie : zombies) {
                if (zombie && zombie->IsAlive()) { // 添加null和存活检查
                    zombie->Update(50, plants, brain); // 确保 brain 对象被传递
                }
            }// delta的原因

            // 更新植物
            for (auto plant : plants) {
                if (plant && plant->IsAlive()) {
                    plant->Update(delta); // 调用通用更新 (例如动画)

                    // 如果是攻击型植物，则调用其攻击逻辑更新
                    AttackPlant* attack_plant = dynamic_cast<AttackPlant*>(plant);
                    if (attack_plant) {
                        attack_plant->UpdateAttackLogic(50, zombies, bullets);
                    }
                }
            }
            // 更新子弹
            for (size_t i = 0; i < bullets.size(); ++i) {
                if (bullets[i] && bullets[i]->IsActive()) {
                    bullets[i]->Update(delta);

                    // 子弹与僵尸的碰撞检测
                    for (Zombie* zombie : zombies) {
                        if (zombie && zombie->IsAlive() && bullets[i] && bullets[i]->IsActive()) {
                            POINT bullet_pos = bullets[i]->GetPosition();
                            POINT zombie_pos = zombie->GetPosition();

                            // 简易碰撞检测 (基于中心点和大致范围，或使用矩形)
                            // 子弹的碰撞区域可以小一点
                            // 僵尸的碰撞区域需要根据 Zombie::ZOMBIE_WIDTH 和 Zombie::ZOMBIE_HEIGHT
                            RECT bullet_rect = { bullet_pos.x, bullet_pos.y, bullet_pos.x + 15, bullet_pos.y + 15 }; // 假设子弹15x15
                            RECT zombie_rect = { zombie_pos.x, zombie_pos.y, zombie_pos.x + Zombie::ZOMBIE_WIDTH, zombie_pos.y + Zombie::ZOMBIE_HEIGHT };

                            RECT intersection; // 用于存储交集矩形
                            if (IntersectRect(&intersection, &bullet_rect, &zombie_rect)) {
                                zombie->TakeDamage(bullets[i]->GetDamage()); // 僵尸受到伤害
                                bullets[i]->Deactivate();                    // 子弹失效

                                break; // 一颗子弹通常只击中一个目标
                            }
                            // 可添加播放击中音效
                        }
                    }
                }
            }
            // 清理不再活动的子弹
            bullets.erase(
                std::remove_if(bullets.begin(), bullets.end(),
                    [](Bullet* b) {
                        if (b && !b->IsActive()) { delete b; return true; }
                        return false;
                    }),
                bullets.end());

            // 回合逻辑处理 (生成新僵尸仅在非休息期发生)
            if (isResting) {
                // 当前是休息期
                    // 检查休息时间是否结束
                if (GetTickCount() - roundOverTime >= REST_PERIOD) {
                    // 休息期结束，开始下一回合

                    if (currentRound > MAX_ROUNDS) {
                        // 所有回合完成，游戏胜利
                        gameState = GAME_VICTORY;
                    }
                    else {
                        // 设置下一回合的参数
                        zombiesToSpawnThisRound = 5 + 10 * currentRound;
                        zombiesSpawnedThisRound = 0;
                        currentZombieSpawnInterval = INITIAL_ZOMBIE_SPAWN_INTERVAL * static_cast<float>(pow(0.8, currentRound - 1));

                        if (spawner) { delete spawner; spawner = nullptr; } // 清理旧的spawner
                        if (!current_tombstone_positions_for_spawner.empty()) {
                            spawner = new ZombieSpawner(current_tombstone_positions_for_spawner, static_cast<int>(currentZombieSpawnInterval), 1.0f);
                        }
                        else {
                            // 错误处理：如果此时没有墓碑位置，则游戏无法继续
                            running = false;
                        }
                        isResting = false; // 进入波数进行状态
                    }
                }
            }
            else {
                // 当前非休息期 (波数正在进行中)
                    // a. 僵尸生成逻辑
                if (spawner != nullptr && zombiesSpawnedThisRound < zombiesToSpawnThisRound) {
                    // 如果spawner存在，且本回合需要生成的僵尸还未全部生成
                    if (Zombie* new_zombie = spawner->Update(delta)) { // spawner的Update决定是否生成僵尸
                        zombies.push_back(new_zombie); // 将新生成的僵尸加入容器
                        zombiesSpawnedThisRound++; // 增加本回合已生成僵尸计数
                    }
                }

                // b. 检查当前波数是否结束
                // 条件：本回合所有预定僵尸已生成完毕，并且场上所有僵尸已被消灭
                if (zombiesSpawnedThisRound >= zombiesToSpawnThisRound && zombies.empty()) {

                    if (currentRound >= MAX_ROUNDS) {
                        // 如果已达到最大回合数，则游戏胜利
                        gameState = GAME_VICTORY;
                    }
                    else {
                        // 当前波数结束，进入休息期
                        gameState = UPGRADE_CHOICE;
                        if (spawner) { delete spawner; spawner = nullptr; } // 清理当前波数的spawner，下一波会新建
                    }
                }
            }
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
            
            putimage_alpha(0, 0, seedbank->frame_list[0]);

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
            putimage_alpha(0, 0, seedbank->frame_list[0]);

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
            putimage_alpha(0, 0, seedbank->frame_list[0]);
            // 显示已选择的tombstone
            for (const auto& tomb : tombstones) {
                tomb->draw();
            }

            // 显示提示文字
            drawChineseText(WIDTH / 2 - 150, 50, _T("点击选择基地位置"), 40, RGB(255, 255, 255));

            break;
        }
        case PLAYING: {
            // 绘制游戏界面
            cleardevice();
            putimage_alpha(0, 0, backgroundAtlas->frame_list[0]);
            putimage_alpha(0, 0, seedbank->frame_list[0]);
            // 显示tombstone
            for (const auto& tomb : tombstones) {
                tomb->draw();
            }

            // 显示基地
            if (basePosition.x >= 0 && basePosition.y >= 0) {
                brain->Draw();
            }

            // 在这里可以添加更多游戏内容
            drawChineseText(1100, 10, _T("游戏进行中 - 按ESC暂停"), 20, RGB(255, 255, 255));
            // 绘制游戏对象
            brain->Draw();              // 绘制大脑基地
            for (auto plant : plants) { // 绘制植物
                plant->Draw();
            }
            for (Bullet* bullet : bullets) {
                if (bullet && bullet->IsActive()) bullet->Draw(); // 子弹
            }
            for (auto zombie : zombies) { // 绘制僵尸
                zombie->Draw();
            }

			// ***********************绘制UI层******************//
            // 阳光数量
            setfillcolor(RGB(255, 215, 0));
            settextcolor(RGB(0, 0, 0));
            TCHAR sun_text[20];
            _stprintf_s(sun_text, _T("%d"), sun_count);
            RECT text_rect = { 20, 60, 63, 80 };

            drawtext(sun_text, &text_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            // 大脑血量
            TCHAR hp_text[20];
            _stprintf_s(hp_text, _T("大脑血量: %d"), brain->GetHP());
            outtextxy(1100, 50, hp_text);

            // 僵尸数量（调试信息）
            TCHAR zombie_text[20];
            _stprintf_s(zombie_text, _T("僵尸数量: %d"), (int)zombies.size());
            outtextxy(1100, 90, zombie_text);

            // 回合信息显示
            TCHAR text_buffer[20];
            if (isResting) { // 如果是休息期
                // 计算并显示下一波倒计时
                DWORD time_left_ms = (roundOverTime + REST_PERIOD) > GetTickCount() ? (roundOverTime + REST_PERIOD - GetTickCount()) : 0;
                _stprintf_s(text_buffer, _T("下一波 %d 秒后"), time_left_ms / 1000);
                drawChineseText(WIDTH / 2 - 150, 20, text_buffer, 30, YELLOW); // 休息提示用黄色
            }
            else { // 如果是波数进行中
                // 显示当前波数和总波数
                _stprintf_s(text_buffer, _T("第 %d / %d 波"), currentRound, MAX_ROUNDS);
                drawChineseText(WIDTH / 2 - 100, 20, text_buffer, 30, WHITE);
                // 显示本波还需生成的僵尸数量 (如果还有未生成的)
                if (zombiesToSpawnThisRound - zombiesSpawnedThisRound > 0) {
                    _stprintf_s(text_buffer, _T("本波待生成: %d"), zombiesToSpawnThisRound - zombiesSpawnedThisRound);
                    drawChineseText(WIDTH / 2 - 100, 50, text_buffer, 20, WHITE);
                }
            }
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
            if (selected_plant >= 1 && selected_plant <= 5) {
                setlinestyle(PS_SOLID, 3);
                setlinecolor(GREEN); // 设置边框颜色为白色

                double rect_width_float = 49.89;
                double rect_height_float = 67.66;
                double top_y_float = 9.83;
                double left_x_float = 0.0;

                switch (selected_plant) {
                case 1: left_x_float = 85.80; break;
                case 2: left_x_float = 151.56; break;
                case 3: left_x_float = 216.95; break;
                case 4: left_x_float = 283.47; break;
                case 5: left_x_float = 350.00; break;
                }

                // 计算矩形的整数坐标
                // EasyX的rectangle函数需要整数坐标
                int r_left = static_cast<int>(left_x_float);
                int r_top = static_cast<int>(top_y_float);
                int r_right = static_cast<int>(left_x_float + rect_width_float);
                int r_bottom = static_cast<int>(top_y_float + rect_height_float);

                rectangle(r_left, r_top, r_right, r_bottom);
            }
            break;
        }
        case PAUSED: {
            // 绘制暂停界面
            cleardevice();
            putimage_alpha(0, 0, backgroundAtlas->frame_list[0]);
            putimage_alpha(0, 0, seedbank->frame_list[0]);
            // 显示游戏内容
            for (const auto& tomb : tombstones) {
                tomb->draw();
            }
            if (basePosition.x >= 0 && basePosition.y >= 0) {
                putimage_alpha(basePosition.x, basePosition.y, brainBaseAtlas->frame_list[0]);
            }
            int buttonX = 390;
            int buttonY = 110;
            putimage_alpha(buttonX, buttonY, pauseButtonAtlas->frame_list[0]);
            break;
        }
        // 绘制强化选择界面
        case UPGRADE_CHOICE: {
            cleardevice();
            // 绘制背景和种子栏 (如果需要保持一致的背景)
            putimage_alpha(0, 0, backgroundAtlas->frame_list[0]);
            putimage_alpha(0, 0, seedbank->frame_list[0]);

            // 绘制提示文字
            drawChineseText(WIDTH / 2 - 200, HEIGHT / 2 - 150, _T("回合结束，请选择强化！"), 40, YELLOW);

            // 定义强化选项按钮的布局参数 (与输入处理部分保持一致)
            int button_width = 600;
            int button_height = 50;
            int button_spacing = 20;
            int button_x = WIDTH / 2 - button_width / 2;
            int button1_y = HEIGHT / 2 - button_height * 2;
            int button2_y = button1_y + button_height + button_spacing;
            int button3_y = button2_y + button_height + button_spacing;

            // 设置按钮样式
            setfillcolor(DARKGRAY); // 按钮背景色
            setlinecolor(WHITE);    // 按钮边框色
            setbkmode(TRANSPARENT); // 文字背景透明

            // 绘制第一个强化选项按钮 (攻击强化)
            solidrectangle(button_x, button1_y, button_x + button_width, button1_y + button_height); // 绘制按钮背景
            rectangle(button_x, button1_y, button_x + button_width, button1_y + button_height);      // 绘制按钮边框
            drawChineseText(button_x + 20, button1_y + 10, _T("强化攻击：豌豆射手与双发射手攻击力 +10"), 25, WHITE);

            // 绘制第二个强化选项按钮 (防御强化)
            solidrectangle(button_x, button2_y, button_x + button_width, button2_y + button_height);
            rectangle(button_x, button2_y, button_x + button_width, button2_y + button_height);
            drawChineseText(button_x + 20, button2_y + 10, _T("强化防御：坚果墙与高坚果生命值 +100"), 25, WHITE);

            // 绘制第三个强化选项按钮 (基地强化)
            solidrectangle(button_x, button3_y, button_x + button_width, button3_y + button_height);
            rectangle(button_x, button3_y, button_x + button_width, button3_y + button_height); // Typo: button_y should be button3_y
            drawChineseText(button_x + 20, button3_y + 10, _T("强化基地：大脑基地生命值 +300"), 25, WHITE);
            break;
        } // 结束 UPGRADE_CHOICE 绘制逻辑
        case GAME_OVER: {
            cleardevice();
            putimage_alpha(0, 0, backgroundAtlas->frame_list[0]);
            putimage_alpha(0, 0, seedbank->frame_list[0]);
            int buttonX = 390;
            int buttonY = 110;

            putimage_alpha(buttonX, buttonY, gameover_botton->frame_list[0]);
            
            break;
        }
        case GAME_VICTORY: {
            putimage_alpha(0, 0, backgroundAtlas->frame_list[0]);
            // 绘制游戏胜利界面
            int buttonX = 390;
            int buttonY = 110;
            putimage_alpha(buttonX, buttonY, victory_botton->frame_list[0]);
            /*drawChineseText(WIDTH / 2 - 150, HEIGHT / 2 - 100, _T("游戏胜利!"), 70, GREEN);
            drawChineseText(WIDTH / 2 - 220, HEIGHT / 2 + 20, _T("恭喜你成功抵御了所有僵尸!"), 30, WHITE);
            drawChineseText(WIDTH / 2 - 150, HEIGHT / 2 + 70, _T("点击任意位置退出"), 25, YELLOW);*/
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
        // 清理子弹
        for (auto b : bullets) delete b; bullets.clear();
        NormalBullet::Cleanup(); 
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

        // 清理tombstones
        for (auto tomb : tombstones) {
            delete tomb;
        }
        tombstones.clear();
        if (spawner != nullptr) {
            delete spawner;
            spawner = nullptr;
        }

        // 清理其他资源
        delete brain;
        delete anim_player_left;
        delete anim_player_right;
        delete atlas_player_left;
        delete atlas_player_right;
        delete backgroundAtlas;
        delete beginButtonAtlas;
        delete button1Atlas;
        delete button2Atlas;
        delete button3Atlas;
        delete brainBaseAtlas;
        delete pauseButtonAtlas;
        delete seedbank;
		delete gameover_botton;
		delete victory_botton;

        closegraph(); // 关闭图形窗口
        return 0;
}