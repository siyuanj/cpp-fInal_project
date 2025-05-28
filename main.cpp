#include <graphics.h>      // EasyXͼ�ο�
#include <string>
#include <iostream>
#include <windows.h>      // Windows API
#include <vector>         // ��׼ģ���vector����
#include <cmath>          // ��ѧ������
#include <conio.h>         // ����̨�������
#include <random>
#include <algorithm>

// �����Զ������ͷ�ļ�
#include "Animation.h"        // ����ϵͳ
#include "Plant.h"           // ֲ�����
#include "SpecificPlants.h"  // ����ֲ����
#include "Bullet.h"          // �ӵ�ϵͳ
#include "Sun.h"             // ����ϵͳ
#include "Zombie.h"          // ��ʬϵͳ
#include "BrainBase.h"       // ���Ի�����
#include "tombstone.h"       // Ĺ����

// ����Windowsͼ����⣬����֧��͸��ͼƬ
#pragma comment(lib, "MSIMG32.LIB")


// ȫ�ֳ����ͱ���
const double PI = 3.1415926;         // Բ���ʳ���
double PLAYER_SPEED = 5.0;           // ����ƶ��ٶ�
POINT player_position = { 500,500 }; // ��ҳ�ʼλ��
int sun_count = 500;                  // ��ʼ��������
#define WIDTH 1280                   // ���ڿ��
#define HEIGHT 720                   // ���ڸ߶�

// ����ȫ�ֵ���Ҷ���ͼ������
Atlas* atlas_player_left;
Atlas* atlas_player_right;

// Ԥ�����tombstoneλ��
struct TombstonePosition {
    int x, y;
    TombstonePosition(int px, int py) : x(px), y(py) {}
};

// ��Ϸ״̬ö��
enum GameState {
    START_SCREEN,
    SELECT_ZOMBIES,
    PLACE_BASE,
    PLAYING,
    PAUSED,
    GAME_OVER,
    GAME_VICTORY, // ��Ϸʤ��״̬
    UPGRADE_CHOICE // �غϽ���ǿ��ѡ��״̬
};

// �����Ƿ��ھ�����
static bool isPointInRect(int px, int py, int x, int y, int width, int height) {
    return px >= x && px <= x + width && py >= y && py <= y + height;
}
// ���������ı�����
static void drawChineseText(int x, int y, const TCHAR* text, int height = 30, COLORREF color = WHITE) {
    setbkmode(TRANSPARENT);
    LOGFONT font;
    gettextstyle(&font);
    font.lfHeight = height;
    _tcscpy_s(font.lfFaceName, _T("΢���ź�"));
    font.lfQuality = ANTIALIASED_QUALITY;
    font.lfWeight = FW_NORMAL;
    settextstyle(&font);
    settextcolor(color);
    outtextxy(x, y, text);
}

int main() {
    std::random_device rd;  // Ӳ����Դ��������������Ԥ�⣩
    std::mt19937 g(rd());   // �� rd() ���������������

    initgraph(WIDTH, HEIGHT); // ��ʼ��ͼ�δ��ڣ����ô�СΪ1280x720
    cleardevice();

    // ����������ʾ��ʽ
    LOGFONT font;
    gettextstyle(&font);
    font.lfHeight = 30;                         // ����߶�
    _tcscpy_s(font.lfFaceName, _T("Arial"));   // ��������
    font.lfQuality = ANTIALIASED_QUALITY;       // �����
    settextstyle(&font);                        // Ӧ����������
    setbkmode(TRANSPARENT);                     // ���ֱ���͸��

    // ��ʼ��ȫ�� Atlas ����
    atlas_player_left = new Atlas(_T("img/player_left_%d.png"), 6);   // ���򶯻�֡
    atlas_player_right = new Atlas(_T("img/player_right_%d.png"), 6);  // ���򶯻�֡

    // ������Ҷ�������
    Animation* anim_player_left = new Animation(atlas_player_left, 45);   // ���򶯻�
    Animation* anim_player_right = new Animation(atlas_player_right, 45); // ���򶯻�

    // ������Դ
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

    POINT tombstone_pos = { 1000, 50 }; // ��ʼĹ��λ��

    backgroundAtlas = new Atlas(_T("img/background.png"));// ����
    beginButtonAtlas = new Atlas(_T("img/begin_idle.png"));// ��ʼ����
    button1Atlas = new Atlas(_T("img/botton_1.png"));// ѡ��Ĺ������1����
    button2Atlas = new Atlas(_T("img/botton_2.png"));// ѡ��Ĺ������2����
    button3Atlas = new Atlas(_T("img/botton_3.png"));// ѡ��Ĺ������3����
    brainBaseAtlas = new Atlas(_T("img/brain_base.png"));// ���Ի���
    pauseButtonAtlas = new Atlas(_T("img/pause_idle.png"));// ��ͣ����
    //sun_back = new Atlas(_T("img/sun_back.png"));// ������
    seedbank = new Atlas(_T("img/seedbank_plant.png"));// ��������
    gameover_botton = new Atlas(_T("img/gameover_eng.png"));// ��������
    victory_botton = new Atlas(_T("img/victory_botton.png"));// ��������


    ExMessage msg;              // ��Ϣ�ṹ�壬���ڴ����û�����

    // ��ʼ����Ϸ״̬�ͱ���
    GameState gameState = START_SCREEN;
    int lastTime = GetTickCount();
    bool running = true;

    // tombstone ����
    int selectedZombieCount = 0;// ѡ���Ĺ������
	std::vector<tombstone*> tombstones; // �洢tombstoneָ������飬���tombstone���ָ������
    POINT basePosition = {-1, -1};// ����λ�ã���ʼΪ��Чֵ
    // Ԥ�����tombstone����λ��
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

    // ---- �غ��ƻ��Ʊ��� START ----
    int currentRound = 0;
    const int MAX_ROUNDS = 5;
    int zombiesToSpawnThisRound = 0;
    int zombiesSpawnedThisRound = 0;
    const float INITIAL_ZOMBIE_SPAWN_INTERVAL = 3000.0f; // ��ʼ��ʬ���ɼ�� (����)
    float currentZombieSpawnInterval = INITIAL_ZOMBIE_SPAWN_INTERVAL;
    std::vector<POINT> current_tombstone_positions_for_spawner; // ���ڴ洢���ݸ� Spawner ��Ĺ��λ��

    bool isResting = false; // �Ƿ��ڻغϼ�Ъ��
    DWORD roundOverTime = 0; // �غϽ���ʱ���
    const DWORD REST_PERIOD = 10000; // ��Ϣʱ�� 10�� (����)

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

    // ��ʼ����ʬϵͳ
    ZombieSpawner* spawner = nullptr; // ����spawnerָ��
    std::vector<Zombie*> zombies;              // ��ʬ����
    // ȫ���ӵ�����
    std::vector<Bullet*> bullets; // ���ڴ洢�����ڳ��ӵ�

    // ����Ϸѭ��
    while (running) {
        DWORD startTime = GetTickCount(); // ��¼֡��ʼʱ��

        // �����Ϸ�Ƿ�ʧ��
        if (!brain->IsAlive()) {
            cleardevice();
			gameState = GAME_OVER; // ������Ϸ״̬Ϊ����
            
        }

        // �����û����롪���׶δ�����
        while (peekmessage(&msg)) {
            switch (gameState) {
            case START_SCREEN: {
                // ��ʼ�������봦��
                if (msg.message == WM_LBUTTONDOWN) {
                    // ����Ƿ����˿�ʼ��Ϸ��ť 
                    // �������жϵ����֮��ͳһ��ͼ
                    int buttonX = WIDTH / 2 - 100;
                    int buttonY = HEIGHT / 2 + 50;
                    if (isPointInRect(msg.x, msg.y, buttonX, buttonY, 250, 250)) {
                        gameState = SELECT_ZOMBIES;
                    }
                }
                break;
            }
            case SELECT_ZOMBIES: {
                // ѡ��ʬ��������
                if (msg.message == WM_LBUTTONDOWN) {
                    int button1X = WIDTH / 2 - 375; // ������ťˮƽ����
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
                        // ����ɵ�tombstone����
                        for (auto t : tombstones) {
                            delete t;
                        }
                        tombstones.clear();  // ���֮ǰ��Ĺ��
                        std::vector<int> indices;
                        for (int i = 0; i < possibleTombstonePositions.size(); i++) {
                            indices.push_back(i);  // ���������б� [0, 1, 2, ..., N-1]
                        }
                        std::shuffle(indices.begin(), indices.end(), g);  // �����������

                        for (int i = 0; i < selectedZombieCount && i < indices.size(); i++) {
                            POINT p = { possibleTombstonePositions[indices[i]].x, possibleTombstonePositions[indices[i]].y };
                            tombstones.push_back(new tombstone(p));
                            current_tombstone_positions_for_spawner.push_back(p); // ��ȷ�ռ�Ĺ��λ��
                        }

                        gameState = PLACE_BASE;  // ������û���״̬
                    }
                }
                break;
            }
            case PLACE_BASE: {
                // ѡ�����λ��
                if (msg.message == WM_LBUTTONDOWN) {
                    // �����λ���Ƿ���tombstone�ص�
                    bool validPosition = true;
                    for (const auto& tomb : tombstones) { // �޸�Ϊ����tombstone����
                        POINT tombPos = tomb->getPosition(); // ��ȡtombstoneλ��
                        // �����λ���Ƿ���tombstone�ص�
                        if (abs(msg.x - tombPos.x) < 200 && abs(msg.y - tombPos.y) < 200) {  // �����ص�
                            validPosition = false;
                            break;
                        }
                    }

                    // ����Ƿ�����Ļ�߽���
                    if (msg.x < 125 || msg.x > WIDTH - 125 || msg.y < 125 || msg.y > HEIGHT - 125) {
                        validPosition = false;
                    }

                    if (validPosition) {
                        basePosition.x = msg.x - 125; // ���з���
                        basePosition.y = msg.y - 125;
                        brain->SetPosition(basePosition); // **����������BrainBase��ʵ��λ��**
                        gameState = PLAYING;
                    }
                    // ---- ��ʼ��һ�غ��߼� START ----
                    currentRound = 1; // ֱ����Ϊ��һ�غ�
                    zombiesToSpawnThisRound = 5 + 10 * currentRound;
                    zombiesSpawnedThisRound = 0;// ���������ɵĽ�ʬ����
                    // ���㱾�غϽ�ʬ���ɼ����ÿ�غϼ���20%��
                    currentZombieSpawnInterval = INITIAL_ZOMBIE_SPAWN_INTERVAL * static_cast<float>(pow(0.8, currentRound - 1));

                    if (spawner) {
                        delete spawner;
                        spawner = nullptr;
                    }
                    if (!current_tombstone_positions_for_spawner.empty()) {
                        spawner = new ZombieSpawner(current_tombstone_positions_for_spawner, static_cast<int>(currentZombieSpawnInterval), 1.0f);
                        // 1.0f ��ʾ100%���ɸ���
						//ÿ�غ����´���spawner���������޸ļ��
                    }
                    else {
                        // ����û��Ĺ��λ�õ�����������ϲ�Ӧ����
                        drawChineseText(WIDTH / 2 - 100, HEIGHT / 2, _T("����û��Ĺ��λ�ã�"), 30, RED);
                        FlushBatchDraw(); Sleep(3000); running = false;
                    }
                    isResting = false;
                    // ---- ��ʼ��һ�غ��߼� END ----
                }
                break;
            }
            case PLAYING: {
                // ��Ϸ�����봦��
                //����������Ƹ��ӣ��������߼��ŵ��׶�ѡ��֮�������д
                //�����������ͣ
                if (msg.message == WM_KEYDOWN && msg.vkcode == VK_ESCAPE) {
                    gameState = PAUSED;
                }
                switch (msg.message) {
                case WM_KEYDOWN:    // ���������¼�
                    switch (msg.vkcode) {
                    case '1': selected_plant = 1; break;  // ѡ�����տ�
                    case '2': selected_plant = 2; break;  // ѡ���㶹����
                    case '3': selected_plant = 3; break;  // ѡ����ǽ
                    case '4': selected_plant = 4; break;  // ѡ��߼��
                    case '5': selected_plant = 5; break;  // ѡ��˫������
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

                case WM_KEYUP:      // �����ͷ��¼�
                    switch (msg.vkcode) {
                    case '1': case '2': case '3': case '4': case '5':selected_plant = 0; break;
                    case 'W': case VK_UP: moving_up = false; break;
                    case 'S': case VK_DOWN: moving_down = false; break;
                    case 'A': case VK_LEFT: moving_left = false; break;
                    case 'D': case VK_RIGHT: moving_right = false; break;
                    }
                    break;

                case WM_LBUTTONDOWN:    // ����������¼�
                    if (selected_plant > 0) {
                        POINT click_pos = { msg.x, msg.y };
                        Plant* new_plant = nullptr; //new_plantΪָ��
                        int cost = 0;

                        // ����ѡ�񴴽���Ӧ��ֲ��
                        switch (selected_plant) {
                        case 1: // ���տ�
                            cost = 50;
                            if (sun_count >= cost) {
                                new_plant = new Sunflower(click_pos);// ʹָ��ָ���½���ֲ�����
                                // new�ؼ��ֵ������Ƕ�̬�����ڴ沢�������ʹ�䲻���ں�������ʱ������
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
                        case 4: // �߼��
                            cost = 125; // ����߼������������Ϊ125������Ը�����Ҫ�޸�
                            if (sun_count >= cost) {
                                new_plant = new TallWallNut(click_pos);
                                sun_count -= cost;
                            }
                            break;
                        case 5: // Repeater
                            cost = 200; // Repeater ����������
                            if (sun_count >= cost) {
                                new_plant = new Repeater(click_pos);
                                sun_count -= cost;
                            }
                            break;
                        }


                        if (new_plant) {
                            plants.push_back(new_plant);// ��ָ�����ֲ��������
                        }
                    }
                    break;
                }
                break;
            }
            case PAUSED: {
                // ��ͣ״̬���봦��
                if (msg.message == WM_KEYDOWN && msg.vkcode == VK_ESCAPE) {
                    running = false;
                }
                else if (msg.message == WM_LBUTTONDOWN) {
                    gameState = PLAYING;
                }
                break;
            }
                       // ����������ǿ��ѡ����������
            case UPGRADE_CHOICE: {
                if (msg.message == WM_LBUTTONDOWN) {
                    // ����ǿ��ѡ�ť�Ĳ��ֲ���
                    int button_width = 600; // ��ť���
                    int button_height = 50;  // ��ť�߶�
                    int button_spacing = 20; // ��ť���
                    int button_x = WIDTH / 2 - button_width / 2; // ��ťX���� (����)
                    // ��ťY���꣬����Ļ�м���������
                    int button1_y = HEIGHT / 2 - button_height * 2;
                    int button2_y = button1_y + button_height + button_spacing;
                    int button3_y = button2_y + button_height + button_spacing;

                    bool choice_made = false; // ����Ƿ�������ѡ��

                    // ����Ƿ����˵�һ��ǿ��ѡ�� (����ǿ��)
                    if (isPointInRect(msg.x, msg.y, button_x, button1_y, button_width, button_height)) {
                        // ��������ֲ��� Peashooter �� Repeater ���ӹ�����
                        for (Plant* p : plants) {
                            // ���� Peashooter �� Repeater �̳��� AttackPlant, 
                            // ���� AttackPlant �� IncreaseAttackPower ����
                            if (dynamic_cast<Peashooter*>(p) || dynamic_cast<Repeater*>(p)) {
                                if (AttackPlant* ap = dynamic_cast<AttackPlant*>(p)) {
                                    ap->IncreaseAttackPower(10); // ������+10
                                }
                            }
                        }
                        choice_made = true;
                    }
                    // ����Ƿ����˵ڶ���ǿ��ѡ�� (����ǿ��)
                    else if (isPointInRect(msg.x, msg.y, button_x, button2_y, button_width, button_height)) {
                        // ��������ֲ��� WallNut �� TallWallNut ��������ֵ
                        for (Plant* p : plants) {
                            // ���� Plant ������ IncreaseMaxHPAndHeal ����
                            if (WallNut* wn = dynamic_cast<WallNut*>(p)) {
                                wn->IncreaseMaxHPAndHeal(100); // ����ֵ+100
                            }
                            else if (TallWallNut* twn = dynamic_cast<TallWallNut*>(p)) {
                                twn->IncreaseMaxHPAndHeal(100); // ����ֵ+100
                            }
                        }
                        choice_made = true;
                    }
                    // ����Ƿ����˵�����ǿ��ѡ�� (����ǿ��)
                    else if (isPointInRect(msg.x, msg.y, button_x, button3_y, button_width, button_height)) {
                        // ���Ӵ��Ի�������ֵ
                        // ���� BrainBase ���� ApplyHealthUpgrade ����
                        brain->ApplyHealthUpgrade(300); // ��������ֵ+300
                        choice_made = true;
                    }

                    // ���������ѡ���������һ�غϵ�׼���׶�
                    if (choice_made) {
                        currentRound++; // �غ�������

                        // Ϊ��һ�غ����ò���
                        zombiesToSpawnThisRound = 5 + 5 * currentRound; // ������һ�غϽ�ʬ����
                        zombiesSpawnedThisRound = 0; // ���������ɽ�ʬ����
                        // ������һ�غϽ�ʬ���ɼ��
                        currentZombieSpawnInterval = INITIAL_ZOMBIE_SPAWN_INTERVAL * static_cast<float>(pow(0.8, currentRound - 1));

                        // �������´�����ʬ������
                        if (spawner) { delete spawner; spawner = nullptr; }
                        if (!current_tombstone_positions_for_spawner.empty()) {
                            spawner = new ZombieSpawner(current_tombstone_positions_for_spawner, static_cast<int>(currentZombieSpawnInterval), 1.0f);
                        }
                        else {
                            // ���������û��Ĺ��λ�ã���Ϸ�����޷�����
                            drawChineseText(WIDTH / 2 - 100, HEIGHT / 2, _T("����û��Ĺ��λ�ã�"), 30, RED);
                            FlushBatchDraw(); Sleep(3000); running = false;
                        }
                        isResting = true; // ����غϼ�Ъ��
                        roundOverTime = GetTickCount(); // ��¼�غϽ�����ǿ��ѡ����ϣ���ʱ�䣬������Ϣ�ڼ�ʱ
                        gameState = PLAYING; // ������Ϸ����״̬
                    }
                }
                break;
            } // ���� UPGRADE_CHOICE ���봦��
            case GAME_OVER: {
                
                if (msg.message == WM_LBUTTONDOWN) {
                    running = false;
                    // ������˳���Ϸѭ��
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

        // *********************************�����������****************************************//
        // *********************************���¶���״̬****************************************//
        BeginBatchDraw(); // ��ʼ������ͼ����ֹ��˸
        // ������Ϸ״̬
        DWORD delta = 1000 / 144;  // ����֡���ʱ��
        //�˴���Ҫ�Ľ�������ϵͳ�����Ż�������ᵼ������ƶ��������

        // �������λ��

        if(gameState == PLAYING){
            if (moving_up) player_position.y -= PLAYER_SPEED;
            if (moving_down) player_position.y += PLAYER_SPEED;
            if (moving_left) player_position.x -= PLAYER_SPEED;
            if (moving_right) player_position.x += PLAYER_SPEED;

            // ��ұ߽���
            if (player_position.x < 0) player_position.x = 0;
            if (player_position.y < 0) player_position.y = 0;
            if (player_position.x > 1280 - 80) player_position.x = 1280 - 81;
            if (player_position.y > 720 - 80) player_position.y = 720 - 81;

            //// ���½�ʬϵͳ
            //if (spawner != nullptr) { // ȷ�� spawner �ѱ�����
            //    if (Zombie* new_zombie = spawner->Update(delta)) {
            //        // ͨ��delta�ж��Ƿ�Ӧ�������µĽ�ʬ,Update��������һ���µĽ�ʬ����ָ��
            //        // ��ʬ���͡�λ�õȾ���Update�����ڲ��߼�����
            //        zombies.push_back(new_zombie);
            //    }
            //}


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
            );// ***********************����﷨������

            //�������н�ʬ
            for (auto zombie : zombies) {
                if (zombie && zombie->IsAlive()) { // ���null�ʹ����
                    zombie->Update(50, plants, brain); // ȷ�� brain ���󱻴���
                }
            }// delta��ԭ��

            // ����ֲ��
            for (auto plant : plants) {
                if (plant && plant->IsAlive()) {
                    plant->Update(delta); // ����ͨ�ø��� (���綯��)

                    // ����ǹ�����ֲ�������乥���߼�����
                    AttackPlant* attack_plant = dynamic_cast<AttackPlant*>(plant);
                    if (attack_plant) {
                        attack_plant->UpdateAttackLogic(50, zombies, bullets);
                    }
                }
            }
            // �����ӵ�
            for (size_t i = 0; i < bullets.size(); ++i) {
                if (bullets[i] && bullets[i]->IsActive()) {
                    bullets[i]->Update(delta);

                    // �ӵ��뽩ʬ����ײ���
                    for (Zombie* zombie : zombies) {
                        if (zombie && zombie->IsAlive() && bullets[i] && bullets[i]->IsActive()) {
                            POINT bullet_pos = bullets[i]->GetPosition();
                            POINT zombie_pos = zombie->GetPosition();

                            // ������ײ��� (�������ĵ�ʹ��·�Χ����ʹ�þ���)
                            // �ӵ�����ײ�������Сһ��
                            // ��ʬ����ײ������Ҫ���� Zombie::ZOMBIE_WIDTH �� Zombie::ZOMBIE_HEIGHT
                            RECT bullet_rect = { bullet_pos.x, bullet_pos.y, bullet_pos.x + 15, bullet_pos.y + 15 }; // �����ӵ�15x15
                            RECT zombie_rect = { zombie_pos.x, zombie_pos.y, zombie_pos.x + Zombie::ZOMBIE_WIDTH, zombie_pos.y + Zombie::ZOMBIE_HEIGHT };

                            RECT intersection; // ���ڴ洢��������
                            if (IntersectRect(&intersection, &bullet_rect, &zombie_rect)) {
                                zombie->TakeDamage(bullets[i]->GetDamage()); // ��ʬ�ܵ��˺�
                                bullets[i]->Deactivate();                    // �ӵ�ʧЧ

                                break; // һ���ӵ�ͨ��ֻ����һ��Ŀ��
                            }
                            // ����Ӳ��Ż�����Ч
                        }
                    }
                }
            }
            // �����ٻ���ӵ�
            bullets.erase(
                std::remove_if(bullets.begin(), bullets.end(),
                    [](Bullet* b) {
                        if (b && !b->IsActive()) { delete b; return true; }
                        return false;
                    }),
                bullets.end());

            // �غ��߼����� (�����½�ʬ���ڷ���Ϣ�ڷ���)
            if (isResting) {
                // ��ǰ����Ϣ��
                    // �����Ϣʱ���Ƿ����
                if (GetTickCount() - roundOverTime >= REST_PERIOD) {
                    // ��Ϣ�ڽ�������ʼ��һ�غ�

                    if (currentRound > MAX_ROUNDS) {
                        // ���лغ���ɣ���Ϸʤ��
                        gameState = GAME_VICTORY;
                    }
                    else {
                        // ������һ�غϵĲ���
                        zombiesToSpawnThisRound = 5 + 10 * currentRound;
                        zombiesSpawnedThisRound = 0;
                        currentZombieSpawnInterval = INITIAL_ZOMBIE_SPAWN_INTERVAL * static_cast<float>(pow(0.8, currentRound - 1));

                        if (spawner) { delete spawner; spawner = nullptr; } // ����ɵ�spawner
                        if (!current_tombstone_positions_for_spawner.empty()) {
                            spawner = new ZombieSpawner(current_tombstone_positions_for_spawner, static_cast<int>(currentZombieSpawnInterval), 1.0f);
                        }
                        else {
                            // �����������ʱû��Ĺ��λ�ã�����Ϸ�޷�����
                            running = false;
                        }
                        isResting = false; // ���벨������״̬
                    }
                }
            }
            else {
                // ��ǰ����Ϣ�� (�������ڽ�����)
                    // a. ��ʬ�����߼�
                if (spawner != nullptr && zombiesSpawnedThisRound < zombiesToSpawnThisRound) {
                    // ���spawner���ڣ��ұ��غ���Ҫ���ɵĽ�ʬ��δȫ������
                    if (Zombie* new_zombie = spawner->Update(delta)) { // spawner��Update�����Ƿ����ɽ�ʬ
                        zombies.push_back(new_zombie); // �������ɵĽ�ʬ��������
                        zombiesSpawnedThisRound++; // ���ӱ��غ������ɽ�ʬ����
                    }
                }

                // b. ��鵱ǰ�����Ƿ����
                // ���������غ�����Ԥ����ʬ��������ϣ����ҳ������н�ʬ�ѱ�����
                if (zombiesSpawnedThisRound >= zombiesToSpawnThisRound && zombies.empty()) {

                    if (currentRound >= MAX_ROUNDS) {
                        // ����Ѵﵽ���غ���������Ϸʤ��
                        gameState = GAME_VICTORY;
                    }
                    else {
                        // ��ǰ����������������Ϣ��
                        gameState = UPGRADE_CHOICE;
                        if (spawner) { delete spawner; spawner = nullptr; } // ����ǰ������spawner����һ�����½�
                    }
                }
            }
        }




        // ������Ϸ����
        cleardevice();  // �����Ļ
        // *********************************��ʼ��ͼ****************************************//
        switch (gameState) {
        case START_SCREEN: {
            // ���ƿ�ʼ����
            cleardevice();
            // ��ʾ����
            putimage_alpha(0, 0, backgroundAtlas->frame_list[0]);
            
            putimage_alpha(0, 0, seedbank->frame_list[0]);

            // ��ʾ��ʼ��Ϸ��ť��ʹ��ͼƬ��
            int buttonX = WIDTH / 2 - 100;
            int buttonY = HEIGHT / 2 + 50;
            putimage_alpha(buttonX, buttonY, beginButtonAtlas->frame_list[0]);
            break;
        }
        case SELECT_ZOMBIES: {
            // ���ƽ�ʬ����ѡ�����
            cleardevice();
            putimage_alpha(0, 0, backgroundAtlas->frame_list[0]);
            putimage_alpha(0, 0, seedbank->frame_list[0]);

            // ��ʾ��ʾ����
            drawChineseText(WIDTH / 2 - 200, HEIGHT / 2 - 250, _T("ѡ��ʬ���ɵ�����"), 40, RGB(255, 255, 255));

            // ��ʾ������ť
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
            // ���ƻ��ط��ý���
            cleardevice();
            putimage_alpha(0, 0, backgroundAtlas->frame_list[0]);
            putimage_alpha(0, 0, seedbank->frame_list[0]);
            // ��ʾ��ѡ���tombstone
            for (const auto& tomb : tombstones) {
                tomb->draw();
            }

            // ��ʾ��ʾ����
            drawChineseText(WIDTH / 2 - 150, 50, _T("���ѡ�����λ��"), 40, RGB(255, 255, 255));

            break;
        }
        case PLAYING: {
            // ������Ϸ����
            cleardevice();
            putimage_alpha(0, 0, backgroundAtlas->frame_list[0]);
            putimage_alpha(0, 0, seedbank->frame_list[0]);
            // ��ʾtombstone
            for (const auto& tomb : tombstones) {
                tomb->draw();
            }

            // ��ʾ����
            if (basePosition.x >= 0 && basePosition.y >= 0) {
                brain->Draw();
            }

            // �����������Ӹ�����Ϸ����
            drawChineseText(1100, 10, _T("��Ϸ������ - ��ESC��ͣ"), 20, RGB(255, 255, 255));
            // ������Ϸ����
            brain->Draw();              // ���ƴ��Ի���
            for (auto plant : plants) { // ����ֲ��
                plant->Draw();
            }
            for (Bullet* bullet : bullets) {
                if (bullet && bullet->IsActive()) bullet->Draw(); // �ӵ�
            }
            for (auto zombie : zombies) { // ���ƽ�ʬ
                zombie->Draw();
            }

			// ***********************����UI��******************//
            // ��������
            setfillcolor(RGB(255, 215, 0));
            settextcolor(RGB(0, 0, 0));
            TCHAR sun_text[20];
            _stprintf_s(sun_text, _T("%d"), sun_count);
            RECT text_rect = { 20, 60, 63, 80 };

            drawtext(sun_text, &text_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            // ����Ѫ��
            TCHAR hp_text[20];
            _stprintf_s(hp_text, _T("����Ѫ��: %d"), brain->GetHP());
            outtextxy(1100, 50, hp_text);

            // ��ʬ������������Ϣ��
            TCHAR zombie_text[20];
            _stprintf_s(zombie_text, _T("��ʬ����: %d"), (int)zombies.size());
            outtextxy(1100, 90, zombie_text);

            // �غ���Ϣ��ʾ
            TCHAR text_buffer[20];
            if (isResting) { // �������Ϣ��
                // ���㲢��ʾ��һ������ʱ
                DWORD time_left_ms = (roundOverTime + REST_PERIOD) > GetTickCount() ? (roundOverTime + REST_PERIOD - GetTickCount()) : 0;
                _stprintf_s(text_buffer, _T("��һ�� %d ���"), time_left_ms / 1000);
                drawChineseText(WIDTH / 2 - 150, 20, text_buffer, 30, YELLOW); // ��Ϣ��ʾ�û�ɫ
            }
            else { // ����ǲ���������
                // ��ʾ��ǰ�������ܲ���
                _stprintf_s(text_buffer, _T("�� %d / %d ��"), currentRound, MAX_ROUNDS);
                drawChineseText(WIDTH / 2 - 100, 20, text_buffer, 30, WHITE);
                // ��ʾ�����������ɵĽ�ʬ���� (�������δ���ɵ�)
                if (zombiesToSpawnThisRound - zombiesSpawnedThisRound > 0) {
                    _stprintf_s(text_buffer, _T("����������: %d"), zombiesToSpawnThisRound - zombiesSpawnedThisRound);
                    drawChineseText(WIDTH / 2 - 100, 50, text_buffer, 20, WHITE);
                }
            }
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
            if (selected_plant >= 1 && selected_plant <= 5) {
                setlinestyle(PS_SOLID, 3);
                setlinecolor(GREEN); // ���ñ߿���ɫΪ��ɫ

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

                // ������ε���������
                // EasyX��rectangle������Ҫ��������
                int r_left = static_cast<int>(left_x_float);
                int r_top = static_cast<int>(top_y_float);
                int r_right = static_cast<int>(left_x_float + rect_width_float);
                int r_bottom = static_cast<int>(top_y_float + rect_height_float);

                rectangle(r_left, r_top, r_right, r_bottom);
            }
            break;
        }
        case PAUSED: {
            // ������ͣ����
            cleardevice();
            putimage_alpha(0, 0, backgroundAtlas->frame_list[0]);
            putimage_alpha(0, 0, seedbank->frame_list[0]);
            // ��ʾ��Ϸ����
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
        // ����ǿ��ѡ�����
        case UPGRADE_CHOICE: {
            cleardevice();
            // ���Ʊ����������� (�����Ҫ����һ�µı���)
            putimage_alpha(0, 0, backgroundAtlas->frame_list[0]);
            putimage_alpha(0, 0, seedbank->frame_list[0]);

            // ������ʾ����
            drawChineseText(WIDTH / 2 - 200, HEIGHT / 2 - 150, _T("�غϽ�������ѡ��ǿ����"), 40, YELLOW);

            // ����ǿ��ѡ�ť�Ĳ��ֲ��� (�����봦���ֱ���һ��)
            int button_width = 600;
            int button_height = 50;
            int button_spacing = 20;
            int button_x = WIDTH / 2 - button_width / 2;
            int button1_y = HEIGHT / 2 - button_height * 2;
            int button2_y = button1_y + button_height + button_spacing;
            int button3_y = button2_y + button_height + button_spacing;

            // ���ð�ť��ʽ
            setfillcolor(DARKGRAY); // ��ť����ɫ
            setlinecolor(WHITE);    // ��ť�߿�ɫ
            setbkmode(TRANSPARENT); // ���ֱ���͸��

            // ���Ƶ�һ��ǿ��ѡ�ť (����ǿ��)
            solidrectangle(button_x, button1_y, button_x + button_width, button1_y + button_height); // ���ư�ť����
            rectangle(button_x, button1_y, button_x + button_width, button1_y + button_height);      // ���ư�ť�߿�
            drawChineseText(button_x + 20, button1_y + 10, _T("ǿ���������㶹������˫�����ֹ����� +10"), 25, WHITE);

            // ���Ƶڶ���ǿ��ѡ�ť (����ǿ��)
            solidrectangle(button_x, button2_y, button_x + button_width, button2_y + button_height);
            rectangle(button_x, button2_y, button_x + button_width, button2_y + button_height);
            drawChineseText(button_x + 20, button2_y + 10, _T("ǿ�����������ǽ��߼������ֵ +100"), 25, WHITE);

            // ���Ƶ�����ǿ��ѡ�ť (����ǿ��)
            solidrectangle(button_x, button3_y, button_x + button_width, button3_y + button_height);
            rectangle(button_x, button3_y, button_x + button_width, button3_y + button_height); // Typo: button_y should be button3_y
            drawChineseText(button_x + 20, button3_y + 10, _T("ǿ�����أ����Ի�������ֵ +300"), 25, WHITE);
            break;
        } // ���� UPGRADE_CHOICE �����߼�
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
            // ������Ϸʤ������
            int buttonX = 390;
            int buttonY = 110;
            putimage_alpha(buttonX, buttonY, victory_botton->frame_list[0]);
            /*drawChineseText(WIDTH / 2 - 150, HEIGHT / 2 - 100, _T("��Ϸʤ��!"), 70, GREEN);
            drawChineseText(WIDTH / 2 - 220, HEIGHT / 2 + 20, _T("��ϲ��ɹ����������н�ʬ!"), 30, WHITE);
            drawChineseText(WIDTH / 2 - 150, HEIGHT / 2 + 70, _T("�������λ���˳�"), 25, YELLOW);*/
            break;
        }
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
        // �����ӵ�
        for (auto b : bullets) delete b; bullets.clear();
        NormalBullet::Cleanup(); 
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

        // ����tombstones
        for (auto tomb : tombstones) {
            delete tomb;
        }
        tombstones.clear();
        if (spawner != nullptr) {
            delete spawner;
            spawner = nullptr;
        }

        // ����������Դ
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

        closegraph(); // �ر�ͼ�δ���
        return 0;
}