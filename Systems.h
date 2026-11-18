#pragma once
#include "raylib.h"
#include "Components.h"
#include "Registry.h"
#include <vector>
#include <queue>
#include <string>

#include <fstream>
#include <iostream> 

#include <cmath>
#include <algorithm>

using namespace std;

// ==========================================
// 3. SYSTEMS
// ==========================================


inline Texture2D LoadGameTexture(const std::string& filePath) {
    // 1. Dosya belirtilen yolda gerçekten var mı?
    if (!FileExists(filePath.c_str())) {
        std::cout << "DOSYA BULUNAMADI: " << filePath 
                  << " | Calisma Dizini: " << GetWorkingDirectory() << std::endl;
        return {0};
    }

    // 2. Dosya var ama Raylib formatı okuyabiliyor mu?
    Texture2D tex = LoadTexture(filePath.c_str());
    if (tex.id == 0) {
        std::cout << "FORMAT HATASI (Dosya gercek PNG degil / WebP olabilir): " << filePath << std::endl;
    } else {
        SetTextureFilter(tex, TEXTURE_FILTER_POINT);
    }
    return tex;
}

inline void DrawCellTexture(Texture2D tex, int gridX, int gridY, int cellSize, Color fallbackColor, bool isCircle = false) {
    int pixelX = gridX * cellSize;
    int pixelY = gridY * cellSize;

    if (tex.id > 0) {
        Rectangle source = { 0.0f, 0.0f, (float)tex.width, (float)tex.height };
        Rectangle dest = { (float)pixelX, (float)pixelY, (float)cellSize, (float)cellSize };
        DrawTexturePro(tex, source, dest, { 0.0f, 0.0f }, 0.0f, WHITE);
    } else {
        if (isCircle) {
            DrawCircle(pixelX + cellSize / 2, pixelY + cellSize / 2, cellSize / 3, fallbackColor);
            DrawCircleLines(pixelX + cellSize / 2, pixelY + cellSize / 2, cellSize / 3, BLACK);
        } else {
            DrawRectangle(pixelX, pixelY, cellSize, cellSize, fallbackColor);
        }
    }
}

// Breadth-First Search (BFS)
inline Position FindNextStep(int startX, int startY, int targetX, int targetY, const vector<vector<int>>& map, int gridSizeF) {

    if (startX == targetX && startY == targetY)
    {
        Position returnPosition;
        returnPosition.x=startX;
        returnPosition.y=startY;

        return returnPosition;
    }
    
    //if (startX == targetX && startY == targetY) return {startX, startY};

    queue<Position> frontier;
    frontier.push({startX, startY});
    
    
    vector<vector<Position>> came_from(gridSizeF, vector<Position>(gridSizeF, {-1, -1}));
    came_from[startY][startX] = {startX, startY};
    

    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};
    
    bool found = false;
    while (!frontier.empty()) {
        Position current = frontier.front();
        frontier.pop();
        
        if (current.x == targetX && current.y == targetY) {
            found = true;
            break;
        }
        
        for (int i = 0; i < 4; i++) {
            int nx = current.x + dx[i];
            int ny = current.y + dy[i];
            
            
            if (nx >= 0 && nx < gridSizeF && ny >= 0 && ny < gridSizeF) {
                if (map[ny][nx] == 0 && came_from[ny][nx].x == -1) {
                    came_from[ny][nx] = current;
                    frontier.push({nx, ny});
                }
            }
        }
    }
    
    if (!found) return {startX, startY}; 
    
    
    Position current = {targetX, targetY};
    while (came_from[current.y][current.x].x != startX || came_from[current.y][current.x].y != startY) {
        current = came_from[current.y][current.x];
    }
    
    return current;
}



inline void SpawnFloatingText(Registry& registry, int gridX, int gridY, int cellSize, const string& text, Color color) {
    uint32_t ftEntity = registry.createEntity();
    float startX = (float)(gridX * cellSize + 4);
    float startY = (float)(gridY * cellSize - 12);
    registry.floatingTexts[ftEntity] = FloatingText{text, startX, startY, color, 0.9f, 0.9f};
}


inline void PlayerInputSystem(Registry& registry, const vector<vector<int>>& map, GameState& currentState, int gridSizeF, Sound hitSound, int cellSize) {
    
    
    for (auto& [entity, pc] : registry.playerControllers) {
        
        if (registry.positions.count(entity) > 0) {
            Position& pos = registry.positions[entity];
            
            
            int targetX = pos.x;
            int targetY = pos.y;

            
            if (IsKeyPressed(KEY_W)) targetY -= 1; // up
            if (IsKeyPressed(KEY_S)) targetY += 1; // down
            if (IsKeyPressed(KEY_A)) targetX -= 1; // left
            if (IsKeyPressed(KEY_D)) targetX += 1; // right

            if (targetX != pos.x || targetY != pos.y) {
                
                // Prevent moving outside the map boundaries (stay between 0 and 19)
                bool isInsideBounds = (targetX >= 0 && targetX < gridSizeF && 
                                       targetY >= 0 && targetY < gridSizeF);
                
                // The square we move to must be a floor (0), not a wall (1).
                if (isInsideBounds && map[targetY][targetX] == 0) {

                    uint32_t targetEntity = registry.getEntityAt(targetX, targetY);

                    if (targetEntity == 0) {
                        // if there is no one next state
                        pos.x = targetX;
                        pos.y = targetY;
                        // turn enemy
                        currentState = GameState::ENEMY_TURN;
                    } 
                    else {
                        // if the squere is full
                        if (registry.stats.count(targetEntity) > 0 && registry.stats.count(entity) > 0) {
                            int dmg = registry.stats[entity].attackPower;
                            
                            PlaySound(hitSound);
                            registry.stats[targetEntity].hp -= dmg; 
                            SpawnFloatingText(registry, targetX, targetY, cellSize, "-" + to_string(dmg), RED);
                            
                            if (registry.stats[targetEntity].hp <= 0) {
                                registry.killEntity(targetEntity);
                            }
                            currentState = GameState::ENEMY_TURN; 
                        }
                    }
                }
            }
        }
    }
}

inline void AISystem(Registry& registry, const vector<vector<int>>& map, GameState& currentState, int gridSizeF, Sound hitSound, int cellSize) {
    
    for (auto& [entity, ai] : registry.aiControllers) {
        if (registry.positions.count(entity) > 0) {
            Position& myPos = registry.positions[entity];
            
            
            if (registry.positions.count(ai.targetEntityId) > 0) {
                //target position
                Position targetPos = registry.positions[ai.targetEntityId];

                int distance = abs(myPos.x - targetPos.x) + abs(myPos.y - targetPos.y);

                // for male enemy
                if (ai.type == AIType::MELEE_CHASER) {
                     //find next step by bfs
                    Position nextStep = FindNextStep(myPos.x, myPos.y, targetPos.x, targetPos.y, map, gridSizeF);
                    uint32_t targetEntity = registry.getEntityAt(nextStep.x, nextStep.y);
                    
                    if (targetEntity == 0) {
                        // if empty step
                        myPos.x = nextStep.x; myPos.y = nextStep.y;
                    } 
                    else if (targetEntity == ai.targetEntityId) {
                        // if not empty
                        if (registry.stats.count(targetEntity) > 0 && registry.stats.count(entity) > 0) {
                            int dmg = registry.stats[entity].attackPower;
                            registry.stats[targetEntity].hp -= dmg;
                            // registry.stats[targetEntity].hp -= registry.stats[entity].attackPower;
                            PlaySound(hitSound);

                            SpawnFloatingText(registry, targetPos.x, targetPos.y, cellSize, "-" + to_string(dmg), RED);
                            if (registry.stats[targetEntity].hp <= 0) registry.killEntity(targetEntity);
                        }
                    }
                }
                else if (ai.type == AIType::RANGED_ARCHER) { // ranged enemy
                    // if in range attack
                    if (distance <= ai.attackRange) {
                        if (registry.stats.count(ai.targetEntityId) > 0 && registry.stats.count(entity) > 0) {

                            int dmg = registry.stats[entity].attackPower;
                            registry.stats[ai.targetEntityId].hp -= dmg;
                            // registry.stats[ai.targetEntityId].hp -= registry.stats[entity].attackPower;
                            PlaySound(hitSound);
                            SpawnFloatingText(registry, targetPos.x, targetPos.y, cellSize, "-" + to_string(dmg), RED);
                            if (registry.stats[ai.targetEntityId].hp <= 0) registry.killEntity(ai.targetEntityId);
                        }
                    } 
                    else {
                        // out of range step
                        Position nextStep = FindNextStep(myPos.x, myPos.y, targetPos.x, targetPos.y, map, gridSizeF);
                        uint32_t targetEntity = registry.getEntityAt(nextStep.x, nextStep.y);
                        if (targetEntity == 0) {
                            myPos.x = nextStep.x; myPos.y = nextStep.y;
                        }
                    }
                }
            
                
            }
        }
    }


    for (auto& [entity, ability] : registry.rangedAbilities) {
        if (ability.currentCooldown > 0) {
            ability.currentCooldown--;
        }
    }
    
    // turn target
    currentState = GameState::PLAYER_TURN;
}

inline void RenderSystem(Registry& registry, int cellSize) {
    
    for (auto& [entity, pos] : registry.positions) {

        // Ground objects (Stairs and items)
        if (registry.stats.count(entity) == 0 && registry.renderables.count(entity) > 0) {
            Renderable& ren = registry.renderables[entity];
            bool isItem = (registry.items.count(entity) > 0);
            DrawCellTexture(ren.texture, pos.x, pos.y, cellSize, ren.color, isItem);

            
            if (registry.stairs.count(entity) > 0 && ren.texture.id == 0) {
                DrawText("S", pos.x * cellSize + 10, pos.y * cellSize + 8, 20, BLACK);
            }
        }

        
        
        // If this entity also has a 'Renderable' component:
        // if (registry.renderables.count(entity) > 0) {
        //     Renderable& ren = registry.renderables[entity]; // take color
            
        //     // Convert grid coordinate (2, 3) to pixel (64, 96)
        //     int pixelX = pos.x * cellSize;
        //     int pixelY = pos.y * cellSize;
            

        //     if (registry.items.count(entity) > 0) {
        //         // draw item
        //         DrawCircle(pixelX + cellSize / 2, pixelY + cellSize / 2, cellSize / 3, ren.color);
        //         DrawCircleLines(pixelX + cellSize / 2, pixelY + cellSize / 2, cellSize / 3, BLACK);
        //     } else {
        //         // draw character
        //         DrawRectangle(pixelX, pixelY, cellSize, cellSize, ren.color);
        //     }
        //     // draw character
        //     // DrawRectangle(pixelX, pixelY, cellSize, cellSize, ren.color);

        //     // hp bar
        //     if (registry.stats.count(entity) > 0) {
        //         Stats& s = registry.stats[entity];

        //         // health bar location
        //         int barWidth = cellSize; 
        //         int barHeight = 5;
        //         int barX = pixelX;
        //         int barY = pixelY - 8;

        //         // calculate health
        //         float hpRatio = (float)s.hp / (float)s.maxHp;
        //         if (hpRatio < 0.0f) hpRatio = 0.0f; // healt can not under zero
        //         // background
        //         DrawRectangle(barX, barY, barWidth, barHeight, DARKGRAY);
        //         //color chose 
        //         Color healthColor = (hpRatio > 0.5f) ? GREEN : RED;

        //         // filled side
        //         DrawRectangle(barX, barY, (int)(barWidth * hpRatio), barHeight, healthColor);

        //         // frame
        //         DrawRectangleLines(barX, barY, barWidth, barHeight, BLACK);

        //         // string hpText = to_string(s.hp) + "/" + to_string(s.maxHp);
        //         // DrawText(hpText.c_str(), pixelX, pixelY - 15, 14, BLACK);
        //     }
        // }

    }

    //Characters (Player and Enemies) + Health Bars
    for (auto& [entity, pos] : registry.positions) {
        if (registry.stats.count(entity) > 0 && registry.renderables.count(entity) > 0) {
            Renderable& ren = registry.renderables[entity];
            DrawCellTexture(ren.texture, pos.x, pos.y, cellSize, ren.color, false);
            
            Stats& s = registry.stats[entity];
            int pixelX = pos.x * cellSize;
            int pixelY = pos.y * cellSize;
            int barWidth = cellSize;
            int barHeight = 5;
            int barX = pixelX;
            int barY = pixelY - 8;
            
            float hpRatio = (float)s.hp / (float)s.maxHp;
            if (hpRatio < 0.0f) hpRatio = 0.0f;
            
            DrawRectangle(barX, barY, barWidth, barHeight, DARKGRAY);
            Color healthColor = (hpRatio > 0.5f) ? GREEN : RED;
            DrawRectangle(barX, barY, (int)(barWidth * hpRatio), barHeight, healthColor);
            DrawRectangleLines(barX, barY, barWidth, barHeight, BLACK);
        }
    }
}

// create new enemy
// inline void SpawnEnemy(Registry& registry, int x, int y, uint32_t targetPlayerId, AIType type, Color color) {
//     uint32_t enemy = registry.createEntity();
//     registry.positions[enemy] = Position{x, y};     
//     registry.renderables[enemy] = Renderable{color};

//     int range = (type == AIType::RANGED_ARCHER) ? 4 : 1;
//     registry.aiControllers[enemy] = AIController{targetPlayerId, type, range};
//     registry.stats[enemy] = Stats{15, 15, 5};
// }

// create new enemy v.2
inline void SpawnEnemy(Registry& registry, int x, int y, uint32_t targetPlayerId, AIType type, Color fallbackColor, Texture2D tex) {
    uint32_t enemy = registry.createEntity();
    registry.positions[enemy] = Position{x, y};     
    registry.renderables[enemy] = Renderable{fallbackColor, tex};
    int range = (type == AIType::RANGED_ARCHER) ? 4 : 1;
    registry.aiControllers[enemy] = AIController{targetPlayerId, type, range};
    registry.stats[enemy] = Stats{15, 15, 5};
}



inline vector<vector<int>> LoadMap(const string& filePath, int gridSize) {


    // create empty map
    vector<vector<int>> map(gridSize, vector<int>(gridSize, 0));
    
    // open file 
    ifstream file(filePath);

    
    if (!file.is_open()) {
        cout << "Error: " << filePath << " file did not found" << endl;
        return map; 
    }

    for (int y = 0; y < gridSize; y++) {
        for (int x = 0; x < gridSize; x++) {
            char c;
            file >> c;
            
            if (c == '1') {
                map[y][x] = 1;
            } else {
                map[y][x] = 0;
            }
        }
    }
    
    file.close();
    return map;
}



inline void ItemPickupSystem(Registry& registry, int cellSize) {
    uint32_t pickedUpItem = 0;

    for (auto& [playerEntity, pc] : registry.playerControllers) {
        if (registry.positions.count(playerEntity) == 0 || registry.stats.count(playerEntity) == 0) continue;
        
        Position playerPos = registry.positions[playerEntity];
        Stats& playerStats = registry.stats[playerEntity];

        for (auto& [itemEntity, item] : registry.items) {
            if (registry.positions.count(itemEntity) > 0) {
                Position itemPos = registry.positions[itemEntity];
                
                if (playerPos.x == itemPos.x && playerPos.y == itemPos.y) {
                    if (item.type == ItemType::HEALTH_POTION) {
                        playerStats.hp = min(playerStats.maxHp, playerStats.hp + item.value);
                        SpawnFloatingText(registry, playerPos.x, playerPos.y, cellSize, "+" + to_string(item.value) + " Health", GREEN);
                    } 
                    else if (item.type == ItemType::ATTACK_BOOST) {
                        
                        playerStats.attackPower += item.value;
                        SpawnFloatingText(registry, playerPos.x, playerPos.y, cellSize, "+" + to_string(item.value) + " power", GOLD);
                    }
                    pickedUpItem = itemEntity;
                    break;
                }
            }
        }
    }

    
    if (pickedUpItem != 0) {
        registry.killEntity(pickedUpItem);
    }
}

// inline void SpawnItem(Registry& registry, int x, int y, ItemType type, int value, Color color) {
//     uint32_t item = registry.createEntity();
//     registry.positions[item] = Position{x, y};
//     registry.renderables[item] = Renderable{color};
//     registry.items[item] = Item{type, value};
// }
// Spawn Item v.2
inline void SpawnItem(Registry& registry, int x, int y, ItemType type, int value, Color fallbackColor, Texture2D tex) {
    uint32_t item = registry.createEntity();
    registry.positions[item] = Position{x, y};
    registry.renderables[item] = Renderable{fallbackColor, tex};
    registry.items[item] = Item{type, value};
}



inline void PlayerAbilitySystem(Registry& registry, const vector<vector<int>>& map, GameState& currentState, int gridSizeF, int cellSize, Sound hitSound) {
    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return;

    int targetX = GetMouseX() / cellSize;
    int targetY = GetMouseY() / cellSize;

    // check map info
    if (targetX < 0 || targetX >= gridSizeF || targetY < 0 || targetY >= gridSizeF) return;
    if (map[targetY][targetX] != 0) return;

    for (auto& [entity, pc] : registry.playerControllers) {
        if (registry.positions.count(entity) == 0 || registry.rangedAbilities.count(entity) == 0) continue;

        Position& pos = registry.positions[entity];
        RangedAbility& ability = registry.rangedAbilities[entity];

        // coolddown check
        if (ability.currentCooldown > 0) return;

        int distance = abs(pos.x - targetX) + abs(pos.y - targetY);
        if (distance > 0 && distance <= ability.range) {
            uint32_t targetEntity = registry.getEntityAt(targetX, targetY);

            
            if (targetEntity != 0 && registry.aiControllers.count(targetEntity) > 0 && registry.stats.count(targetEntity) > 0) {
                registry.stats[targetEntity].hp -= ability.damage;
                PlaySound(hitSound);
                SpawnFloatingText(registry, targetX, targetY, cellSize, "-" + to_string(ability.damage), ORANGE);

                if (registry.stats[targetEntity].hp <= 0) {
                    registry.killEntity(targetEntity);
                }

                ability.currentCooldown = ability.maxCooldown;
                currentState = GameState::ENEMY_TURN;
                break;
            }
        }
    }
}


inline void RenderTargetingSystem(Registry& registry, const vector<vector<int>>& map, int gridSizeF, int cellSize) {
    int hoverX = GetMouseX() / cellSize;
    int hoverY = GetMouseY() / cellSize;

    if (hoverX < 0 || hoverX >= gridSizeF || hoverY < 0 || hoverY >= gridSizeF) return;
    if (map[hoverY][hoverX] != 0) return;

    for (auto& [entity, pc] : registry.playerControllers) {
        if (registry.positions.count(entity) == 0 || registry.rangedAbilities.count(entity) == 0) continue;

        Position pos = registry.positions[entity];
        RangedAbility ability = registry.rangedAbilities[entity];

        if (ability.currentCooldown == 0) {
            int distance = abs(pos.x - hoverX) + abs(pos.y - hoverY);
            if (distance > 0 && distance <= ability.range) {
                uint32_t hoveredEntity = registry.getEntityAt(hoverX, hoverY);
                if (hoveredEntity != 0 && registry.aiControllers.count(hoveredEntity) > 0) {
                    
                    DrawRectangle(hoverX * cellSize, hoverY * cellSize, cellSize, cellSize, Fade(RED, 0.35f));
                    DrawRectangleLinesEx({(float)(hoverX * cellSize), (float)(hoverY * cellSize), (float)cellSize, (float)cellSize}, 3.0f, ORANGE);
                } else {
                    
                    DrawRectangle(hoverX * cellSize, hoverY * cellSize, cellSize, cellSize, Fade(SKYBLUE, 0.3f));
                }
            }
        }
    }
}






inline void UpdateFloatingTextSystem(Registry& registry, float dt) {
    vector<uint32_t> expiredEntities;

    for (auto& [entity, ft] : registry.floatingTexts) {
        ft.pixelY -= 32.0f * dt; 
        ft.lifetime -= dt;

        if (ft.lifetime <= 0.0f) {
            expiredEntities.push_back(entity);
        }
    }

    for (uint32_t id : expiredEntities) {
        registry.killEntity(id);
    }
}

inline void RenderFloatingTextSystem(Registry& registry) {
    for (auto& [entity, ft] : registry.floatingTexts) {
        float alpha = max(0.0f, ft.lifetime / ft.maxLifetime);
        int x = (int)ft.pixelX;
        int y = (int)ft.pixelY;


        DrawText(ft.text.c_str(), x + 1, y + 1, 18, Fade(BLACK, alpha));
        DrawText(ft.text.c_str(), x, y, 18, Fade(ft.color, alpha));
    }
}