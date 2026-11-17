#pragma once
#include "raylib.h"
#include <cstdint> 


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
struct Renderable { Color color; };
struct PlayerController {}; 
struct AIController { uint32_t targetEntityId; AIType type; int attackRange;};
struct Stats { int hp; int maxHp; int attackPower; };

struct Stairs {};


struct Item {
    ItemType type;
    int value;
};

