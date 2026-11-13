#include "raylib.h"
#include <vector>
#include <iostream>
#include <unordered_map>
#include <cstdint> // uint32_t için


using namespace std;


const int GRID_SIZE = 20;
const int CELL_SIZE = 32; // The size of each square in pixels
const int SCREEN_WIDTH = GRID_SIZE * CELL_SIZE;  // 20 * 32 = 640 pixel
const int SCREEN_HEIGHT = GRID_SIZE * CELL_SIZE; // 20 * 32 = 640 pixel


// ==========================================
// 1. COMPONENTS - Data Only
// ==========================================
struct Position {
    int x;
    int y;
};

struct Renderable {
    Color color;
};

struct PlayerController {};

// ==========================================
// 2. REGISTRY
// ==========================================
class Registry {
    private:
        uint32_t nextEntityId = 1;

    public:
        
        unordered_map<uint32_t, Position> positions;
        unordered_map<uint32_t, Renderable> renderables;

        unordered_map<uint32_t, PlayerController> playerControllers;

        // new ID 
        uint32_t createEntity() {
            return nextEntityId++;
        }
};

// ==========================================
// 3. SYSTEMS
// ==========================================
void RenderSystem(Registry& registry) {
    
    for (auto& [entity, pos] : registry.positions) {
        
        // If this entity also has a 'Renderable' component:
        if (registry.renderables.count(entity) > 0) {
            Renderable& ren = registry.renderables[entity]; // take color
            
            // Convert grid coordinate (2, 3) to pixel (64, 96)
            int pixelX = pos.x * CELL_SIZE;
            int pixelY = pos.y * CELL_SIZE;
            
            // draw character
            DrawRectangle(pixelX, pixelY, CELL_SIZE, CELL_SIZE, ren.color);
        }
    }
}


void PlayerInputSystem(Registry& registry, const vector<vector<int>>& map) {
    
    
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
                bool isInsideBounds = (targetX >= 0 && targetX < GRID_SIZE && 
                                       targetY >= 0 && targetY < GRID_SIZE);
                
                // The square we move to must be a floor (0), not a wall (1).
                if (isInsideBounds && map[targetY][targetX] == 0) {
                    pos.x = targetX;
                    pos.y = targetY;
                }
            }
        }
    }
}

int main() {

    // --- map ---
    vector<vector<int>> map(GRID_SIZE, vector<int>(GRID_SIZE, 0));
    map[5][5] = 1; 
    map[5][6] = 1; 
    map[5][7] = 1; 
    map[12][15] = 1; 
    map[13][15] = 1; 
    map[14][15] = 1;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "GridTactics");
    SetTargetFPS(60);

    Registry registry;

    // make player (Entity ID: 1)
    uint32_t player = registry.createEntity();
    registry.positions[player] = Position{2, 3};      // x:2, y:3 coordinate
    registry.renderables[player] = Renderable{BLUE}; 
    registry.playerControllers[player] = PlayerController{};

    // make enemy (Entity ID: 2)
    uint32_t enemy = registry.createEntity();
    registry.positions[enemy] = Position{15, 10};     // x:15, y:10 coordinate
    registry.renderables[enemy] = Renderable{RED};

    while (!WindowShouldClose()){
        PlayerInputSystem(registry, map);


        
        BeginDrawing();
        
        ClearBackground(RAYWHITE);

        // DrawText (text, X coordinate, Y coordinate, Font size, color)
        // DrawText("First test for draw text, hello world.", 190, 200, 20, BLACK);

        for (int y = 0; y < GRID_SIZE; y++) {
            for (int x = 0; x < GRID_SIZE; x++) {
                int pixelX = x * CELL_SIZE;
                int pixelY = y * CELL_SIZE;
                Color cellColor = (map[y][x] == 1) ? DARKGRAY : LIGHTGRAY;
                DrawRectangle(pixelX, pixelY, CELL_SIZE, CELL_SIZE, cellColor);
                DrawRectangleLines(pixelX, pixelY, CELL_SIZE, CELL_SIZE, BLACK);
            }
        }
        
        RenderSystem(registry);

        EndDrawing();
    }

    

    CloseWindow();
    return 0;

}