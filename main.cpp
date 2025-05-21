#include<graphics.h>
#include<string>
#include<iostream>
#include<windows.h>
#include<vector>
#include<cmath>
// why nothing happen??

// 链接图像处理库
#pragma comment(lib, "MSIMG32.LIB")

const double PI = 3.1415926;

double PLAYER_SPEED = 5.0;// 玩家移动速度
POINT player_position = { 500,500 };// 玩家初始位置
int sun_count = 50; // 初始阳光数量

inline void putimage_alpha(int x, int y, IMAGE* img) {
    // 透明绘制函数，使用AlphaBlend支持alpha通道
    // 原因？？？<--- AlphaBlend 用于混合源图像和目标图像的颜色，考虑到alpha通道可以实现透明或半透明效果
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
    /// 图片类，用于加载和管理图片资源
    // 需要输入路径和图片数量
	// 在Animation类前生成
public:
    std::vector<IMAGE*> frame_list;//创建动态指针数组
    Atlas(LPCTSTR path, int num) {
        // 构造函数
        // 读取文件的方式是基于文件命名的规律的，因此需要传入格式化的文件名
        //具体还需要再理解
        TCHAR path_file[256];// 存储每张图片的路径
        for (size_t i = 0; i < num; i++) {
            _stprintf_s(path_file, path, i); //将路径存入path_file数组中
            IMAGE* picture = new IMAGE;// 创建IMAGE指针 picture，用于储存，之后进行loadimage
            loadimage(picture, path_file);//传入路径，加载图片
            // 这里的路径是一个格式化字符串，%d会被替换为i的值
            frame_list.push_back(picture);// 指针存入动态数组中
        }
    }
    ~Atlas() {
        // 析构函数,释放图片资源
        for (size_t i = 0; i < frame_list.size(); i++) {
            delete frame_list[i];// 释放动态数组中的指针
        }
    }
};

class Animation {
	// 动画类，用于处理动画帧的切换和显示
	// 需要传入Atlas对象指针和帧间隔时间
private:
    Atlas* anim_atlas; // 动画帧集合
    int internal; // 帧间隔时间
    int idx_frame = 0; // 当前帧索引
    int timer = 0; // 计时器

public:
    Animation(Atlas* atlas, int jiange) : anim_atlas(atlas), internal(jiange) {
        // 构造函数
        //anim_atlas和atlas均为指针
    }
    ~Animation() {}
    void showimage(int x, int y, int delta) {
        timer += delta;
        if (timer >= internal) {
            // 使用解引用操作符 + 成员访问
            idx_frame = (idx_frame + 1) % anim_atlas->frame_list.size();
            timer = 0;
        }
        // 通过指针数组访问帧数据
        putimage_alpha(x, y, anim_atlas->frame_list[idx_frame]);
    }
};

// 植物基类
class Plant {
protected:
    int hp;                     // 生命值
    POINT position;             // 位置
    int cost;                   // 种植成本
    Animation* anim;            // 动画对象
    bool is_alive;              // 存活状态

public:
    Plant(int init_hp, POINT pos, int init_cost, Atlas* atlas, int frame_interval) 
        : hp(init_hp), position(pos), cost(init_cost), is_alive(true) {
        anim = new Animation(atlas, frame_interval);
    }

    virtual ~Plant() {
        if (anim) delete anim;
    }

    // 纯虚函数，必须由派生类实现
    virtual void Update(int delta) = 0;
    
    // 基本属性访问
    int GetHP() const { return hp; }
    POINT GetPosition() const { return position; }
    int GetCost() const { return cost; }
    bool IsAlive() const { return is_alive; }

    // 受伤害处理
    virtual void TakeDamage(int damage) {
        hp -= damage;
        if (hp <= 0) {
            hp = 0;
            is_alive = false;
        }
    }

    // 渲染植物
    virtual void Draw() {
        if (is_alive && anim) {
            anim->showimage(position.x, position.y, 1000/144);  // 使用固定帧率
        }
    }
};

// 攻击型植物基类
class AttackPlant : public Plant {
protected:
    int attack_power;      // 攻击力
    int attack_range;      // 攻击范围
    int attack_interval;   // 攻击间隔
    int attack_timer;      // 攻击计时器

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

    virtual void Attack() = 0;  // 纯虚函数，由具体的攻击型植物实现
};

// 防御型植物基类
class DefensePlant : public Plant {
protected:
    int defense;           // 防御值
    bool is_priority;      // 是否优先被攻击

public:
    DefensePlant(int init_hp, POINT pos, int init_cost, Atlas* atlas, 
                 int frame_interval, int init_defense, bool priority)
        : Plant(init_hp, pos, init_cost, atlas, frame_interval),
          defense(init_defense), is_priority(priority) {}

    virtual void Update(int delta) override {
        // 防御型植物的基本更新逻辑
    }

    // 重写受伤害处理，考虑防御值
    virtual void TakeDamage(int damage) override {
        int actual_damage = damage - defense;
        if (actual_damage > 0) {
            Plant::TakeDamage(actual_damage);
        }
    }

    bool IsPriority() const { return is_priority; }
};

// 资源型植物基类
class ResourcePlant : public Plant {
protected:
    int resource_rate;     // 资源产生速率
    int resource_amount;   // 每次产生的资源量
    int resource_timer;    // 资源产生计时器

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

    virtual void GenerateResource() = 0;  // 纯虚函数，由具体的资源型植物实现
};

// 子弹基类
class Bullet {
private:
    Animation* anim;        // 动画对象指针
    double speed;          // 移动速度
    POINT position;        // 子弹位置
    bool is_active;        // 是否激活
    static Atlas* bullet_atlas; // 子弹的图集（所有子弹共享）

public:
    Bullet(POINT pos, double init_speed) 
        : position(pos), speed(init_speed), is_active(true){
        anim = new Animation(bullet_atlas, 50);
    }

    virtual ~Bullet() {
        if (anim) delete anim;
    }

    // 更新子弹位置
    virtual void Update(int delta) {
        if (!is_active) return;

        // 向右移动
        position.x += speed * delta / 1000.0;

        // 如果移出屏幕则销毁
        if (position.x > 1280) {
            is_active = false;
        }
    }

    // 绘制子弹
    virtual void Draw() {
        if (is_active && anim) {
            anim->showimage(position.x, position.y, 1000/144);
        }
    }

    
    // 获取子弹位置
    POINT GetPosition() const { return position; }
    
    // 检查子弹是否激活
    bool IsActive() const { return is_active; }
    
    // 停用子弹
    void Deactivate() { is_active = false; }
};

// 普通子弹类
class NormalBullet : public Bullet {
private:
    static IMAGE* bullet_img;  // 静态图片对象，所有普通子弹共享

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

// 初始化静态成员
IMAGE* NormalBullet::bullet_img = nullptr;

// 初始化静态成员
Atlas* Bullet::bullet_atlas = nullptr;

// 阳光类
class Sun {
private:
    POINT position;        // 当前位置
    POINT target;         // 目标位置
    Animation* anim;      // 动画
    bool is_active;       // 是否激活
    const double MOVE_SPEED = 5.0; // 移动速度

public:
    Sun(POINT pos, Atlas* atlas) : position(pos), is_active(true) {
        // 设置阳光计数器位置
        target = { 68, 2 };
        
        // 创建动画
        anim = new Animation(atlas, 50);
    }

    ~Sun() {
        if (anim) delete anim;
    }

    void Update(int delta) {
        if (!is_active) return;

        // 移向目标位置
        double dx = target.x - position.x;
        double dy = target.y - position.y;
        double distance = sqrt(dx*dx + dy*dy);
        
        if (distance < MOVE_SPEED) {
            is_active = false;
            sun_count += 25;  // 收集到阳光
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

// ********************************具体植物**************************************//


// 向日葵类（资源型植物示例）
class Sunflower : public ResourcePlant {
private:
    static Atlas* sun_atlas;  // 阳光动画帧集合
    std::vector<Sun*> suns;   // 产生的阳光

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
        // 从向日葵右侧20像素处产生阳光
        POINT sun_pos = position;
        sun_pos.x += 40;  // 向日葵中心
        sun_pos.y += 40;
        sun_pos.x += 20;  // 再向右20像素
        suns.push_back(new Sun(sun_pos, sun_atlas));
    }

    void Update(int delta) override {
        ResourcePlant::Update(delta);

        // 更新所有阳光
        for (auto sun : suns) {
            sun->Update(delta);
        }

        // 删除不活跃的阳光
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
        // 绘制所有阳光
        for (auto sun : suns) {
            sun->Draw();
        }
    }
};

// 初始化静态成员
Atlas* Sunflower::sun_atlas = nullptr;

// 豌豆射手类（攻击型植物示例）
class Peashooter : public AttackPlant {
public:
    Peashooter(POINT pos)
        : AttackPlant(100, pos, 100, new Atlas(_T("img/peashooter_%d.png"), 12),
                     100, 20, 400, 1500) {}

    void Attack() override {
        // TODO: 实现发射豌豆的逻辑
    }
};

// 坚果墙类（防御型植物示例）
class WallNut : public DefensePlant {
public:
    WallNut(POINT pos)
        : DefensePlant(400, pos, 50, new Atlas(_T("img/wallnut_%d.png"), 15),
                      150, 20, true) {}
};

//******************Atlas对象********************//
// 创建全局的Atlas对象
Atlas* atlas_player_left = new Atlas(_T("img/player_left_%d.png"), 6);
Atlas* atlas_player_right = new Atlas(_T("img/player_right_%d.png"), 6);











//******************Atlas对象********************//

int main() {
    initgraph(1280, 720); // 创建窗口
    
    // 设置文字样式
    LOGFONT font;
    gettextstyle(&font);
    font.lfHeight = 30;  // 设置字体高度
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
    int selected_plant = 0;  // 当前选中的植物类型（0表示未选中）
    std::vector<Plant*> plants;  // 存储所有放置的植物

    // 初始化移动控制变量
    bool facing_left = true; // 角色朝向，true为左，false为右
    bool moving_up = false;
    bool moving_down = false;
    bool moving_left = false;
    bool moving_right = false;

    loadimage(&img_background, _T("img/background.png"));// 加载背景图片
    //loadimage(&test_img, _T("img/SunFlower1.gif"));// 加载测试图片
    loadimage(&sun_back, _T("img/sun_back.png"));// 加载阳光栏
	loadimage(&tombstone, _T("img/tombstone.png"), 150, 150);// 加载墓碑 宽度：150 高度：150


    BeginBatchDraw();// 开启批量绘图
    while (running) {
        DWORD startTime = GetTickCount(); // 节省时间部分

        while (peekmessage(&msg)) {
            // 按键处理部分
            switch (msg.message) {
            case WM_KEYDOWN:
                switch (msg.vkcode) {  // msg.wParam 和msg.vkcode的区别是什么？
                case '1':  // 选择向日葵
                    selected_plant = 1;
                    break;
                case '2':  // 选择豌豆射手
                    selected_plant = 2;
                    break;
                case '3':  // 选择坚果墙
                    selected_plant = 3;
                    break;
                case VK_UP:
                case 'W':          // 新增 W 键
                    moving_up = true;
                    break;
                case VK_DOWN:
                case 'S':          // 新增 S 键
                    moving_down = true;
                    break;
                case VK_LEFT:
                case 'A':          // 新增 A 键
                    moving_left = true;
                    facing_left = true;
                    break;
                case VK_RIGHT:
                case 'D':          // 新增 D 键
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
                    selected_plant = 0;  // 释放数字键时取消选择
                    break;
                case VK_UP:
                case 'W':          // 释放 W 键时停止上移
                    moving_up = false;
                    break;
                case VK_DOWN:
                case 'S':          // 释放 S 键时停止下移
                    moving_down = false;
                    break;
                case VK_LEFT:
                case 'A':          // 释放 A 键时停止左移
                    moving_left = false;
                    break;
                case VK_RIGHT:
                case 'D':          // 释放 D 键时停止右移
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
                        cost = 50;  // 向日葵成本
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
                        cost = 50;  // 坚果墙成本
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
        DWORD delta = 1000 / 144;
        for (auto plant : plants) {
            plant->Update(delta);
        }

        cleardevice();// 清空屏幕，然后绘制新帧
        // 下方为要绘制上去的内容

        putimage(0, 0, &img_background);//放置背景图片于（0，0）
        //putimage_alpha(0, 0, &test_img);//放置测试图片于（0，0）
        putimage_alpha(0, 0, &sun_back);
        putimage_alpha(1000, 50, &tombstone);
        // 绘制所有植物
        for (auto plant : plants) {
            plant->Draw();
        }

        // 绘制阳光数量
        setfillcolor(RGB(255, 215, 0));  // 设置数字颜色为金色
        settextcolor(RGB(0, 0, 0));      // 设置文字颜色为黑色
        TCHAR sun_text[20];
        _stprintf_s(sun_text, _T("%d"), sun_count);
        RECT text_rect = { 68, 2, 120, 32 }; // 定义文本显示区域
        drawtext(sun_text, &text_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE); // 居中对齐显示

        // 使用Animation类显示角色动画
        if (moving_left || moving_right || moving_up || moving_down) {
            //******************只在移动时更新动画****************//
            if (facing_left) {
                anim_player_left->showimage(player_position.x, player_position.y, delta);
            } else {
                anim_player_right->showimage(player_position.x, player_position.y, delta);
            }
        } else {
            // 静止时显示第一帧
            if (facing_left) {
                putimage_alpha(player_position.x, player_position.y, atlas_player_left->frame_list[0]);
            } else {  
                putimage_alpha(player_position.x, player_position.y, atlas_player_right->frame_list[0]);
            }
        }

        // 如果有植物被选中，显示当前选中的植物类型
        if (selected_plant > 0) {
            TCHAR s[20];
            _stprintf_s(s, _T("Selected: %d"), selected_plant);
            outtextxy(10, 10, s);
        }

        FlushBatchDraw();// 刷新屏幕，显示新绘制的内容

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

    closegraph();// 关闭图形窗口
    return 0;
}