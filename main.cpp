#include "raylib.h"
#include <vector>
#include <iostream>
#include <unordered_map>
#include <cstdint> 
#include <queue>
#include <string>
#include "Components.h"
#include "Registry.h"
#include "Systems.h"




using namespace std;


const int GRID_SIZE = 20;
const int CELL_SIZE = 32; // The size of each square in pixels
const int SCREEN_WIDTH = GRID_SIZE * CELL_SIZE;  // 20 * 32 = 640 pixel
const int SCREEN_HEIGHT = GRID_SIZE * CELL_SIZE; // 20 * 32 = 640 pixel

//enum class GameState { PLAYER_TURN, ENEMY_TURN, GAME_OVER_WIN, GAME_OVER_LOSS };

// ==========================================
// 1. COMPONENTS - Data Only
// ==========================================
// struct Position {
//     int x;
//     int y;
// };

// struct Renderable {
//     Color color;
// };

// struct PlayerController {};


// struct AIController {
//     uint32_t targetEntityId;
// };

// struct Stats {
//     int hp;
//     int maxHp;
//     int attackPower;
// };

// ==========================================
// 2. REGISTRY
// ==========================================
// class Registry {
//     private:
//         uint32_t nextEntityId = 1;

//     public:
        
//         unordered_map<uint32_t, Position> positions;
//         unordered_map<uint32_t, Renderable> renderables;

//         unordered_map<uint32_t, PlayerController> playerControllers;

//         unordered_map<uint32_t, AIController> aiControllers;

//         unordered_map<uint32_t, Stats> stats;

//         // new ID 
//         uint32_t createEntity() {
//             return nextEntityId++;
//         }

//         uint32_t getEntityAt(int x, int y) {
//             for (auto& [entity, pos] : positions) {
//                 if (pos.x == x && pos.y == y) return entity; 
//             }
//             return 0; // empty place
//         }

//         // delete it when hp is zero
//         void killEntity(uint32_t entity) {
//             positions.erase(entity);
//             renderables.erase(entity);
//             playerControllers.erase(entity);
//             aiControllers.erase(entity);
//             stats.erase(entity);
//         }

// };

// ==========================================
// 3. SYSTEMS
// ==========================================
// void RenderSystem(Registry& registry) {
    
//     for (auto& [entity, pos] : registry.positions) {
        
//         // If this entity also has a 'Renderable' component:
//         if (registry.renderables.count(entity) > 0) {
//             Renderable& ren = registry.renderables[entity]; // take color
            
//             // Convert grid coordinate (2, 3) to pixel (64, 96)
//             int pixelX = pos.x * CELL_SIZE;
//             int pixelY = pos.y * CELL_SIZE;
            
//             // draw character
//             DrawRectangle(pixelX, pixelY, CELL_SIZE, CELL_SIZE, ren.color);

//             // hp bar
//             if (registry.stats.count(entity) > 0) {
//                 Stats& s = registry.stats[entity];
//                 string hpText = to_string(s.hp) + "/" + to_string(s.maxHp);
//                 DrawText(hpText.c_str(), pixelX, pixelY - 15, 14, BLACK);
//             }
//         }
//     }
// }


// Breadth-First Search (BFS)
// Position FindNextStep(int startX, int startY, int targetX, int targetY, const vector<vector<int>>& map) {

//     if (startX == targetX && startY == targetY)
//     {
//         Position returnPosition;
//         returnPosition.x=startX;
//         returnPosition.y=startY;

//         return returnPosition;
//     }
    
//     //if (startX == targetX && startY == targetY) return {startX, startY};

//     queue<Position> frontier;
//     frontier.push({startX, startY});
    
    
//     vector<vector<Position>> came_from(GRID_SIZE, vector<Position>(GRID_SIZE, {-1, -1}));
//     came_from[startY][startX] = {startX, startY};
    

//     int dx[] = {0, 0, -1, 1};
//     int dy[] = {-1, 1, 0, 0};
    
//     bool found = false;
//     while (!frontier.empty()) {
//         Position current = frontier.front();
//         frontier.pop();
        
//         if (current.x == targetX && current.y == targetY) {
//             found = true;
//             break;
//         }
        
//         for (int i = 0; i < 4; i++) {
//             int nx = current.x + dx[i];
//             int ny = current.y + dy[i];
            
            
//             if (nx >= 0 && nx < GRID_SIZE && ny >= 0 && ny < GRID_SIZE) {
//                 if (map[ny][nx] == 0 && came_from[ny][nx].x == -1) {
//                     came_from[ny][nx] = current;
//                     frontier.push({nx, ny});
//                 }
//             }
//         }
//     }
    
//     if (!found) return {startX, startY}; 
    
    
//     Position current = {targetX, targetY};
//     while (came_from[current.y][current.x].x != startX || came_from[current.y][current.x].y != startY) {
//         current = came_from[current.y][current.x];
//     }
    
//     return current;
// }

// void PlayerInputSystem(Registry& registry, const vector<vector<int>>& map, GameState& currentState) {
    
    
//     for (auto& [entity, pc] : registry.playerControllers) {
        
//         if (registry.positions.count(entity) > 0) {
//             Position& pos = registry.positions[entity];
            
            
//             int targetX = pos.x;
//             int targetY = pos.y;

            
//             if (IsKeyPressed(KEY_W)) targetY -= 1; // up
//             if (IsKeyPressed(KEY_S)) targetY += 1; // down
//             if (IsKeyPressed(KEY_A)) targetX -= 1; // left
//             if (IsKeyPressed(KEY_D)) targetX += 1; // right

//             if (targetX != pos.x || targetY != pos.y) {
                
//                 // Prevent moving outside the map boundaries (stay between 0 and 19)
//                 bool isInsideBounds = (targetX >= 0 && targetX < GRID_SIZE && 
//                                        targetY >= 0 && targetY < GRID_SIZE);
                
//                 // The square we move to must be a floor (0), not a wall (1).
//                 if (isInsideBounds && map[targetY][targetX] == 0) {

//                     uint32_t targetEntity = registry.getEntityAt(targetX, targetY);

//                     if (targetEntity == 0) {
//                         // if there is no one next state
//                         pos.x = targetX;
//                         pos.y = targetY;
//                         // turn enemy
//                         currentState = GameState::ENEMY_TURN;
//                     } 
//                     else {
//                         // if the squere is full
//                         if (registry.stats.count(targetEntity) > 0 && registry.stats.count(entity) > 0) {
//                             int dmg = registry.stats[entity].attackPower;
//                             registry.stats[targetEntity].hp -= dmg; 
                            
//                             if (registry.stats[targetEntity].hp <= 0) {
//                                 registry.killEntity(targetEntity);
//                             }
//                             currentState = GameState::ENEMY_TURN; 
//                         }
//                     }
//                 }
//             }
//         }
//     }
// }

// void AISystem(Registry& registry, const vector<vector<int>>& map, GameState& currentState) {
    
//     for (auto& [entity, ai] : registry.aiControllers) {
//         if (registry.positions.count(entity) > 0) {
//             Position& myPos = registry.positions[entity];
            
            
//             if (registry.positions.count(ai.targetEntityId) > 0) {
//                 //target position
//                 Position targetPos = registry.positions[ai.targetEntityId];
                
//                 //find next step by bfs
//                 Position nextStep = FindNextStep(myPos.x, myPos.y, targetPos.x, targetPos.y, map);
                

//                 uint32_t targetEntity = registry.getEntityAt(nextStep.x, nextStep.y);
                
//                 if (targetEntity == 0) {
//                     // if empty step
//                     myPos.x = nextStep.x;
//                     myPos.y = nextStep.y;
//                 }
//                 else if (targetEntity == ai.targetEntityId) {
//                     // if not empty
//                     if (registry.stats.count(targetEntity) > 0 && registry.stats.count(entity) > 0) {
//                         int dmg = registry.stats[entity].attackPower;
//                         registry.stats[targetEntity].hp -= dmg;
                        
//                         if (registry.stats[targetEntity].hp <= 0) {
//                             registry.killEntity(targetEntity);
//                         }
//                     }
//                 }
//             }
//         }
//     }
    
//     // // turn target
//     currentState = GameState::PLAYER_TURN;
// }


// create new enemy
// void SpawnEnemy(Registry& registry, int x, int y, uint32_t targetPlayerId) {
//     uint32_t enemy = registry.createEntity();
//     registry.positions[enemy] = Position{x, y};     
//     registry.renderables[enemy] = Renderable{RED};
//     registry.aiControllers[enemy] = AIController{targetPlayerId};
//     registry.stats[enemy] = Stats{15, 15, 5};
// }



void SpawnStairs(Registry& registry, int x, int y) {
    uint32_t stairs = registry.createEntity();
    registry.positions[stairs] = Position{x, y};
    registry.renderables[stairs] = Renderable{GOLD}; 
    //registry.renderables.add_stairs_tag_if_needed... 
    
}

int main() {

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "GridTactics");
    SetTargetFPS(60);
    
    InitAudioDevice();

    Sound hitSound = LoadSound("../assets/hit.wav");
    //Music bgm = LoadMusicStream("../assets/bgm.ogg");
    //PlayMusicStream(bgm);



    // 1. 2D DOKULARI GPU BELLEĞİNE YÜKLE
    GameTextures textures;
    textures.floor       = LoadGameTexture("../assets/floor.png");
    textures.wall        = LoadGameTexture("../assets/wall.png");
    textures.player      = LoadGameTexture("../assets/player.png");
    textures.meleeEnemy  = LoadGameTexture("../assets/melee_enemy.png");
    textures.archerEnemy = LoadGameTexture("../assets/archer_enemy.png");
    textures.potion      = LoadGameTexture("../assets/potion.png");
    textures.boost       = LoadGameTexture("../assets/boost.png");
    textures.stairs      = LoadGameTexture("../assets/stairs.png");

    // --- map ---
    // vector<vector<int>> map(GRID_SIZE, vector<int>(GRID_SIZE, 0));

    // map[5][5] = 1; map[6][5] = 1; map[7][5] = 1; map[8][5] = 1;
    // map[8][6] = 1; map[8][7] = 1; map[8][8] = 1; map[8][9] = 1;
    
    // map[12][15] = 1; map[13][15] = 1; map[14][15] = 1;
    






    

    Registry registry;
    int currentLevel = 1; // whic floor


    auto LoadLevel = [&](int level) {
        registry.clearWorld();
        
        string mapFile = (level == 1) ? "../map.txt" : "../map2.txt";
        auto loadedMap = LoadMap(mapFile, GRID_SIZE);

        // make player (Entity ID: 1)
        uint32_t player = registry.createEntity();
        registry.positions[player] = Position{1, 1};      
        registry.renderables[player] = Renderable{BLUE, textures.player};  
        registry.playerControllers[player] = PlayerController{};
        registry.stats[player] = Stats{100, 100, 15};
        registry.rangedAbilities[player] = RangedAbility{4, 12, 3, 0};

        
        // create stairs
        uint32_t stairs = registry.createEntity();
        registry.positions[stairs] = Position{18, 18};
        registry.renderables[stairs] = Renderable{GOLD, textures.stairs};
        registry.stairs[stairs] = Stairs{};


        
        if (level == 1) {
            SpawnEnemy(registry, 15, 2, player, AIType::MELEE_CHASER, RED, textures.meleeEnemy);
            SpawnEnemy(registry, 18, 10, player, AIType::RANGED_ARCHER, PURPLE, textures.archerEnemy);

            SpawnItem(registry, 5, 1, ItemType::ATTACK_BOOST, 5, ORANGE, textures.boost); 
            SpawnItem(registry, 12, 9, ItemType::HEALTH_POTION, 20, LIME, textures.potion);
        } else {
            //second floor
            SpawnEnemy(registry, 8, 3, player, AIType::MELEE_CHASER, RED, textures.meleeEnemy);
            SpawnEnemy(registry, 14, 9, player, AIType::RANGED_ARCHER, PURPLE, textures.archerEnemy);
            SpawnEnemy(registry, 10, 18, player, AIType::RANGED_ARCHER, PURPLE, textures.archerEnemy);

            SpawnItem(registry, 18, 1, ItemType::ATTACK_BOOST, 5, ORANGE, textures.boost);
            SpawnItem(registry, 3, 9, ItemType::HEALTH_POTION, 25, LIME, textures.potion);
            SpawnItem(registry, 9, 9, ItemType::HEALTH_POTION, 25, LIME, textures.potion);
        }

        return loadedMap;
    };

    vector<vector<int>> map = LoadLevel(currentLevel);

    // make player (Entity ID: 1)
    // uint32_t player = registry.createEntity();
    // registry.positions[player] = Position{2, 3};      // x:2, y:3 coordinate
    // registry.renderables[player] = Renderable{BLUE}; 
    // registry.playerControllers[player] = PlayerController{};
    // registry.stats[player] = Stats{50, 50, 10};
    

    // make enemy (Entity ID: 2)
    // uint32_t enemy = registry.createEntity();
    // registry.positions[enemy] = Position{15, 10};     // x:15, y:10 coordinate
    // registry.renderables[enemy] = Renderable{RED};
    // registry.aiControllers[enemy] = AIController{player};
    // registry.stats[enemy] = Stats{20, 20, 5};

    //SpawnEnemy(registry, 15, 2, player, AIType::MELEE_CHASER, RED);
    // ranged
    //SpawnEnemy(registry, 18, 10, player, AIType::RANGED_ARCHER, PURPLE);
    //SpawnEnemy(registry, 10, 18, player, AIType::MELEE_CHASER, RED);

    GameState currentState = GameState::PLAYER_TURN;

    while (!WindowShouldClose()){

        //UpdateMusicStream(bgm);

        if (registry.playerControllers.empty() && currentState != GameState::GAME_OVER_LOSS) {
            currentState = GameState::GAME_OVER_LOSS;
        }
        else if (registry.aiControllers.empty() && currentState != GameState::GAME_OVER_WIN) {
            
            for (auto& [entity, pos] : registry.positions) {
                if (registry.playerControllers.count(entity) > 0) {
                    
                    if (pos.x == 18 && pos.y == 18) {
                        if (currentLevel == 1) {
                            currentLevel = 2;
                            map = LoadLevel(currentLevel);
                            currentState = GameState::PLAYER_TURN;
                        } else {
                            currentState = GameState::GAME_OVER_WIN;
                        }
                    }
                }
            }
        }

        // if (registry.playerControllers.empty() && currentState != GameState::GAME_OVER_LOSS) {
        //     currentState = GameState::GAME_OVER_LOSS;
        // }
        // else if (registry.aiControllers.empty() && currentState != GameState::GAME_OVER_WIN) {
        //     currentState = GameState::GAME_OVER_WIN;
        // }


        if (currentState == GameState::PLAYER_TURN) {
            PlayerInputSystem(registry, map, currentState, GRID_SIZE,hitSound, CELL_SIZE);

            if (currentState == GameState::PLAYER_TURN) {
                PlayerAbilitySystem(registry, map, currentState, GRID_SIZE, CELL_SIZE, hitSound);
            }

            ItemPickupSystem(registry,CELL_SIZE);
        } 
        else if (currentState == GameState::ENEMY_TURN) {
            
            WaitTime(0.15f); 
            AISystem(registry, map, currentState, GRID_SIZE,hitSound, CELL_SIZE);
            // AISystem(registry, map, currentState, GRID_SIZE, hitSocket = hitSound, GRID_SIZE);
        }
        //PlayerInputSystem(registry, map);
        UpdateFloatingTextSystem(registry, GetFrameTime());
        

        BeginDrawing();
        
        ClearBackground(RAYWHITE);


        
        // DrawText (text, X coordinate, Y coordinate, Font size, color)
        // DrawText("First test for draw text, hello world.", 190, 200, 20, BLACK);

        // for (int y = 0; y < GRID_SIZE; y++) {
        //     for (int x = 0; x < GRID_SIZE; x++) {
        //         int pixelX = x * CELL_SIZE;
        //         int pixelY = y * CELL_SIZE;
        //         Color cellColor = (map[y][x] == 1) ? DARKGRAY : LIGHTGRAY;
                
        //         DrawRectangle(pixelX, pixelY, CELL_SIZE, CELL_SIZE, cellColor);
        //         DrawRectangleLines(pixelX, pixelY, CELL_SIZE, CELL_SIZE, BLACK);
        //     }
        // }

        for (int y = 0; y < GRID_SIZE; y++) {
            for (int x = 0; x < GRID_SIZE; x++) {
                bool isWall = (map[y][x] == 1);
                Texture2D tileTex = isWall ? textures.wall : textures.floor;
                Color fallbackColor = isWall ? DARKGRAY : LIGHTGRAY;

                DrawCellTexture(tileTex, x, y, CELL_SIZE, fallbackColor, false);
                
                // Doku yüklü değilse kare sınır çizgilerini koru
                if (tileTex.id == 0) {
                    DrawRectangleLines(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, BLACK);
                }
            }
        }

        DrawRectangle(18 * CELL_SIZE, 18 * CELL_SIZE, CELL_SIZE, CELL_SIZE, GOLD);
        DrawText("M", 18 * CELL_SIZE + 10, 18 * CELL_SIZE + 8, 20, BLACK);


        
        RenderSystem(registry,CELL_SIZE);
        RenderFloatingTextSystem(registry);

        if (currentState == GameState::PLAYER_TURN) {
            RenderTargetingSystem(registry, map, GRID_SIZE, CELL_SIZE);
        }

        // string levelText = "dungeon floor: " + to_string(currentLevel);
        // DrawText(levelText.c_str(), 10, 10, 20, DARKBLUE);

        int playerHp = 0;
        int playerMaxHp = 0;
        int playerAtk = 0;
        int spellCd = 0;
        for (auto& [entity, pc] : registry.playerControllers) {
            if (registry.stats.count(entity) > 0) {
                playerHp = registry.stats[entity].hp;
                playerMaxHp = registry.stats[entity].maxHp;
                playerAtk = registry.stats[entity].attackPower;
            }
            if (registry.rangedAbilities.count(entity) > 0) {
                spellCd = registry.rangedAbilities[entity].currentCooldown;
            }
        }

        


        DrawRectangle(0, 0, SCREEN_WIDTH, 28, Fade(BLACK, 0.65f));
        string statusText = "dungeon floor: " + to_string(currentLevel) + 
                                 " | Remaining Enemy: " + to_string(registry.aiControllers.size()) +
                                 " | health: " + to_string(playerHp) + "/" + to_string(playerMaxHp) +
                                 " | power: " + to_string(playerAtk);

        //string statusText = "dungeon floor: " + to_string(currentLevel) + " | Remaining Enemy: " + to_string(registry.aiControllers.size());
        DrawText(statusText.c_str(), 10, 6, 18, WHITE);

        DrawRectangle(0, SCREEN_HEIGHT - 28, SCREEN_WIDTH, 28, Fade(BLACK, 0.75f));
        if (spellCd == 0) {
            DrawText("Fireball (Left-Click): READY [Range: 4 | Damage: 12]", 10, SCREEN_HEIGHT - 22, 16, SKYBLUE);
        } else {
            std::string cdText = "Fireball: Cooldown (" + std::to_string(spellCd) + " round)";
            DrawText(cdText.c_str(), 10, SCREEN_HEIGHT - 22, 16, LIGHTGRAY);
        }


        if (currentState == GameState::GAME_OVER_WIN) {

            const char* winText = "YOU WON! THE DUNGEON HAS BEEN CLEARED.";
            int fontSize = 10;
            int textWidth = MeasureText(winText, fontSize);
            int centerX = (SCREEN_WIDTH - textWidth) / 2;

            DrawRectangle(0, SCREEN_HEIGHT/2 - 50, SCREEN_WIDTH, 100, Fade(BLACK, 0.8f));
            DrawText(winText, centerX, SCREEN_HEIGHT/2 - 15, fontSize, GREEN);
        }
        else if (currentState == GameState::GAME_OVER_LOSS) {
            const char* loseText = "YOU DIED!... GAME OVER";
            int fontSize = 10;
            int textWidth = MeasureText(loseText, fontSize);
            int centerX = (SCREEN_WIDTH - textWidth) / 2;

            DrawRectangle(0, SCREEN_HEIGHT/2 - 50, SCREEN_WIDTH, 100, Fade(BLACK, 0.8f));
            DrawText(loseText, centerX, SCREEN_HEIGHT/2 - 15, fontSize, RED);
        }

        EndDrawing();
    }

    if (textures.floor.id > 0) UnloadTexture(textures.floor);
    if (textures.wall.id > 0) UnloadTexture(textures.wall);
    if (textures.player.id > 0) UnloadTexture(textures.player);
    if (textures.meleeEnemy.id > 0) UnloadTexture(textures.meleeEnemy);
    if (textures.archerEnemy.id > 0) UnloadTexture(textures.archerEnemy);
    if (textures.potion.id > 0) UnloadTexture(textures.potion);
    if (textures.boost.id > 0) UnloadTexture(textures.boost);
    if (textures.stairs.id > 0) UnloadTexture(textures.stairs);

    UnloadSound(hitSound);
    //UnloadMusicStream(bgm);
    CloseAudioDevice();
    
    CloseWindow();
    return 0;

}