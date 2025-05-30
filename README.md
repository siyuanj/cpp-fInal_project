# README

## C++ 课程大作业 - Plants vs. Zombies: Rogue Defense

本文档全面概述了基于 EasyX 图形库构建的 C++ 僵尸生存游戏。这是一款塔防风格的生存游戏，玩家必须通过策略性地放置各种植物，保护一个中心基地免受一波又一波僵尸的攻击，并通过回合间的强化来提升防御能力。

## 1. 游戏概述

该游戏挑战玩家保卫他们的“大脑基地”，抵御无情的僵尸部落。玩家可以在地图上移动一个角色来收集资源、选择并放置防御性和攻击性的植物。生存取决于植物的策略性布局、有效的资源管理以及在每个回合结束后做出的强化选择。游戏经历几个阶段，从开始菜单和设置阶段到核心游戏循环（包含多个回合和回合间的强化选择），当玩家的基地被摧毁或成功抵御所有波数攻击后结束。游戏还会记录玩家的胜负次数。

## 2. 玩法介绍

### 游戏目标

主要目标是保护**大脑基地**不被僵尸摧毁。如果基地的生命值降至零，游戏结束。 成功抵御所有设定的僵尸波数（默认为5波）则获得胜利。

### 游戏流程

游戏通过一个明确的状态机进行管理，主要流程如下：

1.  **开始界面 (START_SCREEN)**:
    * 游戏以一个主菜单开始。
    * 显示“开始游戏”和“退出游戏”按钮。
    * 显示历史胜负场次。
    * 提供基本操作指南。
    * 点击“开始游戏”继续，点击“退出游戏”则关闭程序。

2.  **选择难度 (SELECT_ZOMBIES)**:
    * 玩家选择本局游戏中僵尸生成点（墓碑）的数量（1、2或3个）。
    * 墓碑数量越多，游戏难度越高。

3.  **放置基地 (PLACE_BASE)**:
    * 选择难度后，玩家必须通过鼠标左键点击地图上的一个有效位置来放置自己的大脑基地。
    * 需避免将基地放置得离墓碑或屏幕边缘太近。

4.  **游戏进行中 (PLAYING)**:
    * 基地放置完毕，游戏正式开始，进入第一回合。
    * 僵尸将开始从墓碑中生成并向基地移动。
    * 玩家控制角色移动，收集由向日葵产生的“太阳”货币，用于购买新植物。
    * 策略性地放置植物来保卫基地。
    * 游戏总共 `MAX_ROUNDS`（默认为5）个回合。
    * 每回合需要消灭一定数量的僵尸。

5.  **回合结束强化选择 (UPGRADE_CHOICE)**:
    * 成功消灭当前波数的所有僵尸后（且未达到最大波数时），进入强化选择界面。
    * 玩家可以在提供的强化选项中选择一个，例如：
        * 强化攻击植物的攻击力。
        * 强化防御植物的生命值。
        * 强化大脑基地的生命值。
    * 选择后，游戏进入短暂的休息期，然后自动开始下一波攻击。

6.  **暂停界面 (PAUSED)**:
    * 在 `PLAYING` 状态下，按 `ESC` 键可暂停游戏。
    * 在暂停界面，点击屏幕可恢复游戏；再次按 `ESC` 键将清理当前游戏会话并返回到 `START_SCREEN`。

7.  **游戏结束 (GAME_OVER / GAME_VICTORY)**:
    * 若大脑基地生命值降至零，游戏进入 `GAME_OVER` 状态（失败）。
    * 若成功抵御所有波数的僵尸，游戏进入 `GAME_VICTORY` 状态（胜利）。
    * 在结束界面点击屏幕退出游戏。胜败场次会被记录在 `game_stats.txt` 文件中。

### 操作指南

可阅读“用户手册.pdf”获得更详细的讲解。演示视频为"演示视频.mp4"。

-   **玩家移动**: 使用 `W, A, S, D` 或 `方向键` 来移动玩家角色。角色的朝向会根据移动方向改变。
-   **植物选择**:
    -   `1`: 选择向日葵 (花费: 50 太阳)
    -   `2`: 选择豌豆射手 (花费: 100 太阳)
    -   `3`: 选择坚果墙 (花费: 50 太阳)
    -   `4`: 选择高坚果 (花费: 125 太阳)
    -   `5`: 选择双发射手 (花费: 200 太阳)
-   **放置植物**: 选择植物后（需按住对应数字键），**鼠标左键点击** 游戏区域以放置它。你必须有足够的太阳才能购买该植物。
-   **暂停/恢复/退出暂停**: `ESC` 键。

## 3. 游戏特色

### 游戏实体

#### 植物 (Plants)

植物是玩家主要的防御手段。它们通过一个基础的 `Plant` 类创建，并被组织成不同的功能类别。

-   **资源植物 (`Sunflower`)**: 生产太阳，游戏的主要资源。
-   **攻击植物** (`Peashooter`, `Repeater`): 自动攻击在其攻击范围内的僵尸。
    -   `Peashooter`: 发射一颗豌豆。
    -   `Repeater`: 快速连续发射两颗豌豆。
-   **防御植物 (`WallNut`, `TallWallNut`)**: 高生命值的植物，作为屏障阻挡僵尸。僵尸会优先攻击这些植物。

#### 僵尸 (Zombies)

僵尸从墓碑中生成，并向玩家的基地前进。

-   **目标AI**: 僵尸有明确的攻击优先级。它们首先攻击高优先级的防御植物（如 `WallNut`），然后是最近的非优先植物，如果没有其他目标，则最终攻击大脑基地。
-   **`NormalZombie`**: 标准僵尸。
-   **`EliteZombie`**: 比普通僵尸更强大的精英类型，具有更高的生命值和攻击力。
-   **`ArmoredZombie` (`ConeZombie`, `BucketZombie`)**: 这些僵尸有一层额外的防御（盔甲），必须先摧毁盔甲才能对它们的本体生命值造成伤害。当它们的盔甲完好时，它们有独特的外观。

#### 其他游戏对象

-   **`BrainBase`**: 需要防御的核心目标。它有1000点初始生命值和一个可见的血条。
-   **`Bullet`**: 由攻击植物发射的抛射物。它们朝目标移动，并在命中后被移除。
-   **`Sun`**: 向日葵产生的资源。每收集一个太阳，玩家的总数增加25。
-   **`tombstone`**: 作为僵尸生成点的游戏内对象。

### 核心机制与技术特性

-   **回合制与升级系统**: 游戏包含多个回合（波数），每成功防御一波后，玩家可以在多个强化选项中选择一项来增强植物或基地，体现了轻度Rogue元素。
-   **难度选择**: 游戏开始时，玩家可以通过选择墓碑数量来调整游戏难度。
-   **游戏统计**: 自动保存和加载玩家的胜负场次到 `game_stats.txt` 文件。
-   **动画系统**: 游戏使用一个自定义的动画系统（`Animation` 和 `Atlas` 类）来处理所有游戏实体的精灵动画。它支持从单个图像图集加载多帧精灵。
-   **游戏状态管理**: `main.cpp` 中的一个状态机 (`enum GameState`) 控制游戏的流程，从开始界面到游戏结束界面。
-   **面向对象设计**: 代码采用C++类进行结构化，为植物和僵尸设计了清晰的继承层次结构，从而促进了代码复用和可扩展性。
-   **资源管理**:
    * 游戏对象的创建使用动态内存，并在游戏循环中和游戏会话结束时通过`cleanupPreviousGameSession`及主函数末尾进行清理，以防止内存泄漏。
    * 对于如僵尸、子弹等可能大量重复的对象的图像资源，采用了静态成员（`static Atlas*`）的方式进行管理。这些资源由类的静态方法（如 `Zombie::LoadResources()` 和 `NormalBullet::Cleanup()`）统一加载和卸载，确保每个图像集只加载一次，有效避免了重复加载导致的内存开销和潜在泄漏。

## 4. 代码结构

该项目被组织成多个类，每个类都有对应的头文件 (`.h`) 和实现文件 (`.cpp`)。详细说明位于“设计说明书.pdf”

| **文件**                | **描述**                                                     |
| ----------------------- | ------------------------------------------------------------ |
| `main.cpp`              | 包含主游戏循环、窗口初始化、事件处理、游戏状态逻辑（包括回合制和升级选择）、以及渲染调用。 |
| `Animation.h/.cpp`      | 定义了用于加载图像序列的 `Atlas` 类和用于显示逐帧动画的 `Animation` 类。 |
| `Plant.h/.cpp`          | 定义了抽象基类 `Plant` 及其主要的派生类：`AttackPlant`、`DefensePlant` 和 `ResourcePlant`。 |
| `SpecificPlants.h/.cpp` | 实现了具体的植物类，如 `Sunflower`、`Peashooter`、`Repeater`、`WallNut` 和 `TallWallNut`。 |
| `Zombie.h/.cpp`         | 定义了 `Zombie` 基类、`ArmoredZombie` 子类以及具体的僵尸类型（如 `NormalZombie`, `EliteZombie`, `ConeZombie`, `BucketZombie`）。还包括管理僵尸生成的 `ZombieSpawner` 类。 |
| `Bullet.h/.cpp`         | 实现了植物发射的抛射物 `Bullet` 类及其派生类 `NormalBullet`。 |
| `BrainBase.h/.cpp`      | 定义了玩家需要防御的基地 `BrainBase` 类。                    |
| `Sun.h/.cpp`            | 定义了作为可收集资源的 `Sun` 对象。                          |
| `tombstone.h`           | 定义了 `tombstone` 类，它作为僵尸生成点的视觉标记。          |
| `game_stats.txt`        | 存储玩家的胜负场次记录。                                     |

### UML 类图

```mermaid
classDiagram
    direction TD

    subgraph AnimationSystem
        class Atlas {
          +frame_list: vector~IMAGE*~
        }
        class Animation {
          -anim_atlas: Atlas*
          +showimage(int, int, int)
        }
        Atlas "1" *-- "many" IMAGE : contains
        Animation "1" *-- "1" Atlas : uses
    end

    subgraph Projectiles
        class Bullet {
            <<abstract>>
            #position: POINT
            #target_position: POINT
            #speed: double
            #is_active: bool
            #damage_value: int
            +Update(int)
            +Draw()*
        }
        class NormalBullet {
          -static Atlas* bullet_atlas
        }
        Bullet <|-- NormalBullet
        NormalBullet ..> Atlas : uses static
    end

    subgraph PlantsAndSun
        class Plant {
            <<abstract>>
            #hp: int
            #max_hp: int
            #position: POINT
            #cost: int
            #anim: Animation*
            #is_alive: bool
            +TakeDamage(int)
            +Draw()*
            +Update(int)*
            +IncreaseMaxHPAndHeal(int)
        }
        Plant --o Animation

        class AttackPlant {
            <<abstract>>
            #attack_power: int
            #attack_range: int
            #attack_interval: int
            +UpdateAttackLogic(int, vector~Zombie*~, vector~Bullet*~)
            +Attack(Zombie*, vector~Bullet*~)*
            +IncreaseAttackPower(int)
        }
        Plant <|-- AttackPlant
        AttackPlant ..> Bullet : creates

        class Peashooter
        class Repeater
        AttackPlant <|-- Peashooter
        AttackPlant <|-- Repeater
        Peashooter ..> NormalBullet : creates
        Repeater ..> NormalBullet : creates multiple

        class DefensePlant {
            #defense: int
            #is_priority: bool
            +IsPriority(): bool
        }
        Plant <|-- DefensePlant
        class WallNut
        class TallWallNut
        DefensePlant <|-- WallNut
        DefensePlant <|-- TallWallNut

        class ResourcePlant {
            <<abstract>>
            #resource_rate: int
            #resource_amount: int
            +GenerateResource()*
        }
        Plant <|-- ResourcePlant

        class Sunflower {
            -suns: vector~Sun*~
            -static Atlas* sun_atlas
        }
        ResourcePlant <|-- Sunflower
        class Sun {
            #anim: Animation*
            #position: POINT
            #is_active: bool
        }
        Sun --o Animation
        Sunflower o--> Sun : creates & manages
    end

    subgraph Facilities
        class BrainBase {
            -hp: int
            -max_hp: int
            -position: POINT
            #brain_base_atlas: Atlas*
            +TakeDamage(int)
            +ApplyHealthUpgrade(int)
            +Reset()
        }
        BrainBase --o Atlas

        class tombstone {
            #position: POINT
            #tomb_atlas: Atlas*
        }
        tombstone --o Atlas
    end

    subgraph ZombiesAndSpawner
        class Zombie {
            <<abstract>>
            #hp: int
            #max_hp: int
            #attack_power: int
            #position: POINT
            #speed: double
            #is_alive: bool
            #anim: Animation*
            #target_plant: Plant*
            #target_brain: BrainBase*
            #FindNearestTarget(vector~Plant*~, BrainBase*)
            +TakeDamage(int)
            +Draw()
            +Update(int, vector~Plant*~, BrainBase*)
            +static LoadResources()
            +static UnloadResources()
        }
        Zombie --o Animation
        Zombie ..> Plant : targets & attacks
        Zombie ..> BrainBase : targets & attacks
        Zombie ..> Atlas : uses static (atlas_normal_zombie, etc.)

        class NormalZombie
        class EliteZombie
        Zombie <|-- NormalZombie
        Zombie <|-- EliteZombie

        class ArmoredZombie {
            #armor_hp: int
            #max_armor_hp: int
            #has_armor: bool
            #armor_anim: Animation*
        }
        ArmoredZombie --o Animation : uses second for armor
        Zombie <|-- ArmoredZombie

        class ConeZombie
        class BucketZombie
        ArmoredZombie <|-- ConeZombie
        ArmoredZombie <|-- BucketZombie

        class ZombieSpawner {
            -spawn_positions: vector~POINT~
            -base_spawn_interval: int
            -current_spawn_interval: int
            -spawn_chance: double
        }
        ZombieSpawner ..> tombstone : uses position
        ZombieSpawner ..> NormalZombie : creates
        ZombieSpawner ..> EliteZombie : creates
        ZombieSpawner ..> ConeZombie : creates
        ZombieSpawner ..> BucketZombie : creates
    end

    class GameMain {
        <<Orchestrator in main.cpp>>
        -plants: vector~Plant*~
        -zombies: vector~Zombie*~
        -bullets: vector~Bullet*~
        -tombstones: vector~tombstone*~
        -brain: BrainBase*
        -spawner: ZombieSpawner*
        -gameState: GameState
        -sun_count: int
        -currentRound: int
        -games_won: int
        -games_lost: int
    }
    GameMain o--> Plant
    GameMain o--> Zombie
    GameMain o--> Bullet
    GameMain o--> tombstone
    GameMain o--> BrainBase
    GameMain o--> ZombieSpawnerclassDiagram
    direction LR

    class Plant {
        <<Abstract>>
        #hp: int
        #max_hp: int
        #position: POINT
        #cost: int
        #anim: Animation*
        #is_alive: bool
        +TakeDamage(int)
        +Draw()*
        +Update(int)*
    }

    class AttackPlant {
        #attack_power: int
        #attack_range: int
        #attack_interval: int
        +UpdateAttackLogic(int, vector~Zombie*~, vector~Bullet*~)
        +Attack(Zombie*, vector~Bullet*~)*
    }

    class DefensePlant {
        #defense: int
        #is_priority: bool
        +IsPriority(): bool
    }

    class ResourcePlant {
        #resource_rate: int
        #resource_amount: int
        +GenerateResource()*
    }

    Plant <|-- AttackPlant
    Plant <|-- DefensePlant
    Plant <|-- ResourcePlant

    class Peashooter {
    }
    class Repeater {
    }
    AttackPlant <|-- Peashooter
    AttackPlant <|-- Repeater

    class WallNut {
    }
    class TallWallNut {
    }
    DefensePlant <|-- WallNut
    DefensePlant <|-- TallWallNut

    class Sunflower {
        -suns: vector~Sun*~
    }
    ResourcePlant <|-- Sunflower
    Sunflower o--> Sun

    class Zombie {
        #hp: int
        #attack_power: int
        #position: POINT
        #speed: double
        #is_alive: bool
        #target_plant: Plant*
        #target_brain: BrainBase*
        #FindNearestTarget(vector~Plant*~, BrainBase*)
        +TakeDamage(int)
        +Draw()
        +Update(int, vector~Plant*~, BrainBase*)
    }

    class ArmoredZombie {
        #armor_hp: int
        #has_armor: bool
    }
    Zombie <|-- ArmoredZombie

    class NormalZombie {
    }
    class EliteZombie {
    }
    Zombie <|-- NormalZombie
    Zombie <|-- EliteZombie

    class ConeZombie {
    }
    class BucketZombie {
    }
    ArmoredZombie <|-- ConeZombie
    ArmoredZombie <|-- BucketZombie

    class Bullet {
        #position: POINT
        #target_position: POINT
        #speed: double
        #is_active: bool
        #damage_value: int
        +Update(int)
        +Draw()*
    }
    class NormalBullet{
    }
    Bullet <|-- NormalBullet

    class Main {
        -plants: vector~Plant*~
        -zombies: vector~Zombie*~
        -bullets: vector~Bullet*~
        -tombstones: vector~tombstone*~
        -brain: BrainBase*
        -spawner: ZombieSpawner*
        -gameState: GameState
    }

    Main o--> Plant
    Main o--> Zombie
    Main o--> Bullet
    Main o--> BrainBase
    Main o--> ZombieSpawner
    Main o--> tombstone

    ZombieSpawner o--> Zombie
    AttackPlant ..> Bullet : creates
    Zombie ..> Plant : attacks
    Zombie ..> BrainBase : attacks

    class Atlas {
      +frame_list: vector~IMAGE*~
    }
    class Animation {
      -anim_atlas: Atlas*
      +showimage(int, int, int)
    }
    Animation o--> Atlas

    Plant o--> Animation
    Zombie o--> Animation

    class BrainBase{
        -hp: int
        -position: POINT
        +TakeDamage(int)
    }
    class Sun{
        -position: POINT
        -is_active: bool
    }
    class tombstone{
        -position: POINT
    }