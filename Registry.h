#pragma once
#include <unordered_map>
#include <cstdint>
#include "Components.h"

// ==========================================
// 2. REGISTRY
// ==========================================


class Registry {
private:
    uint32_t nextEntityId = 1;

public:
    std::unordered_map<uint32_t, Position> positions;
    std::unordered_map<uint32_t, Renderable> renderables;
    std::unordered_map<uint32_t, PlayerController> playerControllers;
    std::unordered_map<uint32_t, AIController> aiControllers;
    std::unordered_map<uint32_t, Stats> stats; 
    std::unordered_map<uint32_t, Stairs> stairs;

    std::unordered_map<uint32_t, Item> items;
    std::unordered_map<uint32_t, RangedAbility> rangedAbilities;
    std::unordered_map<uint32_t, FloatingText> floatingTexts;

    // new ID 
    uint32_t createEntity() { return nextEntityId++; }
    
    uint32_t getEntityAt(int x, int y) {
        for (auto& [entity, pos] : positions) {

            if (stairs.count(entity) > 0 || items.count(entity) > 0) continue;
            if (pos.x == x && pos.y == y) return entity;
        }
        return 0;// empty place
    }
    // delete it when hp is zero
    void killEntity(uint32_t entity) {
        positions.erase(entity);
        renderables.erase(entity);
        playerControllers.erase(entity);
        aiControllers.erase(entity);
        stats.erase(entity);
        stairs.erase(entity);
        items.erase(entity);
        rangedAbilities.erase(entity);
        floatingTexts.erase(entity);
    }

    
    void clearWorld() {
        positions.clear();
        renderables.clear();
        playerControllers.clear();
        aiControllers.clear();
        stats.clear();
        stairs.clear();
        items.clear();
        rangedAbilities.clear();
        floatingTexts.clear();
    }
};