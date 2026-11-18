#pragma once
#include "raylib.h"
#include <cstdint> 
#include <string>


// ==========================================
// 1. COMPONENTS - Data Only
// ==========================================

enum class GameState { PLAYER_TURN, ENEMY_TURN, GAME_OVER_WIN, GAME_OVER_LOSS };

enum class AIType {
    MELEE_CHASER, 
    RANGED_ARCHER 
};



enum class ItemType {
    HEALTH_POTION, 
    ATTACK_BOOST  
};

struct Position { int x; int y; };


struct Renderable { Color color; Texture2D texture = {0}; };
struct PlayerController {}; 
struct AIController { uint32_t targetEntityId; AIType type; int attackRange;};
struct Stats { int hp; int maxHp; int attackPower; };

struct Stairs {};


struct Item {
    ItemType type;
    int value;
};

struct RangedAbility {
    int range;           
    int damage;          
    int maxCooldown;     
    int currentCooldown; 
};



struct FloatingText {
    std::string text;
    float pixelX;
    float pixelY;
    Color color;
    float lifetime;    
    float maxLifetime; 
};




struct GameTextures {
    Texture2D floor = {0};
    Texture2D wall = {0};
    Texture2D player = {0};
    Texture2D meleeEnemy = {0};
    Texture2D archerEnemy = {0};
    Texture2D potion = {0};
    Texture2D boost = {0};
    Texture2D stairs = {0};
};