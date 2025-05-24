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
int sun_count = 200;                  // ��ʼ��������
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
    GAME_OVER
};

// �����Ƿ��ھ�����
bool isPointInRect(int px, int py, int x, int y, int width, int height) {
    return px >= x && px <= x + width && py >= y && py <= y + height;
}
// ���������ı�����
void drawChineseText(int x, int y, const TCHAR* text, int height = 30, COLORREF color = WHITE) {
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
    Atlas* gameover_botton = nullptr;

    POINT tombstone_pos = { 1000, 50 }; // ��ʼĹ��λ��

    backgroundAtlas = new Atlas(_T("img/background.png"));// ����
    beginButtonAtlas = new Atlas(_T("img/begin_idle.png"));// ��ʼ����
    button1Atlas = new Atlas(_T("img/botton_1.png"));// ѡ��Ĺ������1����
    button2Atlas = new Atlas(_T("img/botton_2.png"));// ѡ��Ĺ������2����
    button3Atlas = new Atlas(_T("img/botton_3.png"));// ѡ��Ĺ������3����
    brainBaseAtlas = new Atlas(_T("img/brain_base.png"));// ���Ի���
    pauseButtonAtlas = new Atlas(_T("img/pause_idle.png"));// ��ͣ����
    sun_back = new Atlas(_T("img/sun_back.png"));// ������
    gameover_botton = new Atlas(_T("img/gameover_eng.png"));// ������

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

                        // ����ѡ��Ľ�ʬ�������Ӵ��ҵ�������ѡȡ��Ӧ������λ��
                        for (int i = 0; i < selectedZombieCount; i++) {
                            // ����tombstone������ӵ�������
                            POINT p = { possibleTombstonePositions[indices[i]].x, possibleTombstonePositions[indices[i]].y };
                            tombstones.push_back(new tombstone(p));
                        }

                        std::vector<POINT> current_tombstone_positions;
                        for (const auto& t_ptr : tombstones) {
                            if (t_ptr) { // ȷ��ָ����Ч
                                current_tombstone_positions.push_back(t_ptr->getPosition());
                                // ��ָ���������
                            }
                        }

                        // ��ʬ���ɳ��д���Ĺ��λ��
                        if (spawner == nullptr) { // ����ǵ�һ�δ���
                            // �������ɼ��Ϊ 1000 ���� �����ɸ���Ϊ 0.5
                            // ����Ҫ������ЩĬ��ֵ
                            spawner = new ZombieSpawner(current_tombstone_positions, 1000, 0.5);
                        }
                        else { // ����Ѿ����ڣ��������Ĺ��λ��
                            spawner->UpdateSpawnPositions(current_tombstone_positions);
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
                    gameState = GAME_OVER;
                }
                else if (msg.message == WM_LBUTTONDOWN) {
                    gameState = PLAYING;
                }
                break;
            }
            case GAME_OVER: {
                running = false;
                /*settextcolor(RED);
                settextstyle(60, 0, _T("Arial"));
                outtextxy(480, 300, _T("��Ϸ����!"));
                FlushBatchDraw();
                Sleep(2000);*/
                
                
                //continue;
                break;
            }
            }
        }

        // *********************************�����������****************************************//

        BeginBatchDraw(); // ��ʼ������ͼ����ֹ��˸
        // ������Ϸ״̬
        DWORD delta = 1000 / 144;  // ����֡���ʱ��
        //�˴���Ҫ�Ľ�������ϵͳ�����Ż�������ᵼ������ƶ��������

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
        if (spawner != nullptr) { // ȷ�� spawner �ѱ�����
            if (Zombie* new_zombie = spawner->Update(delta)) {
				// ͨ��delta�ж��Ƿ�Ӧ�������µĽ�ʬ,Update��������һ���µĽ�ʬ����ָ��
				// ��ʬ���͡�λ�õȾ���Update�����ڲ��߼�����
                zombies.push_back(new_zombie);
            }
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
        // �������н�ʬ
        for (auto zombie : zombies) {
            if (zombie && zombie->IsAlive()) { // ���null�ʹ����
                zombie->Update(50, plants, brain); // ȷ�� brain ���󱻴���
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
            putimage_alpha(0, 0, sun_back->frame_list[0]);

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
            putimage_alpha(0, 0, sun_back->frame_list[0]);

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
            putimage_alpha(0, 0, sun_back->frame_list[0]);
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
            putimage_alpha(0, 0, sun_back->frame_list[0]);
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
            for (auto zombie : zombies) { // ���ƽ�ʬ
                zombie->Draw();
            }

			// ***********************����UI��******************//
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

            break;
        }
        case PAUSED: {
            // ������ͣ����
            cleardevice();
            putimage_alpha(0, 0, backgroundAtlas->frame_list[0]);
            putimage_alpha(0, 0, sun_back->frame_list[0]);
            // ��ʾ��Ϸ����
            for (const auto& tomb : tombstones) {
                tomb->draw();
            }
            if (basePosition.x >= 0 && basePosition.y >= 0) {
                putimage_alpha(basePosition.x, basePosition.y, brainBaseAtlas->frame_list[0]);
            }
            int buttonX = WIDTH / 2 - 100;
            int buttonY = HEIGHT / 2 + 50;
            putimage_alpha(buttonX, buttonY, pauseButtonAtlas->frame_list[0]);
            break;
        }
        case GAME_OVER: {
            int buttonX = WIDTH / 2 - 100;
            int buttonY = HEIGHT / 2 + 50;

            putimage_alpha(buttonX, buttonY, gameover_botton->frame_list[0]);
            if (msg.message == WM_LBUTTONDOWN) {
                break; // ������˳���Ϸѭ��
            }
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
    delete sun_back;

    closegraph(); // �ر�ͼ�δ���
    return 0;
}