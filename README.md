# GridTactics

A modular, turn-based tactical grid RPG built from scratch in **C++** and **Raylib**, powered by a custom **Entity-Component-System (ECS)** architecture.

---

## Features

- **Custom ECS Architecture:** Clean separation between pure data (`Components.h`), entity lifecycle & spatial indexing (`Registry.h`), and decoupled game logic (`Systems.h`).
- **Turn-Based Tactical Combat:**
  - **Melee Combat:** Classic bump-to-attack mechanics on a 2D grid.
  - **Ranged Abilities:** Mouse-targeted *Fireball* spell with tile highlighting, Manhattan distance range validation, and turn-based cooldown tracking.
- **Archetype-Driven Enemy AI:**
  - **Melee Chaser:** Uses **Breadth-First Search (BFS)** pathfinding to navigate around walls and pursue the player.
  - **Ranged Archer:** Evaluates Manhattan distance to hold position and attack once the player enters firing range.
- **Data-Driven Multi-Floor Dungeons:** Loads 20×20 grid layouts from external `.txt` files (`map.txt`, `map2.txt`) with floor transitions unlocked after clearing all enemies and reaching the stairs.
- **Loot & Stat Progression:** Walk-over collectibles including *Health Potions* (restores HP up to max capacity) and *Power Orbs* (permanently increases attack damage).
- **Visual & Audio Feedback:**
  - 2D pixel-art texture rendering (`TEXTURE_FILTER_POINT`) with automatic geometric shape fallbacks when texture assets are omitted.
  - Dynamic overhead health bars and frame-rate-independent floating combat text (`-10`, `-12`, `+20 CAN`).
  - Integrated Raylib audio pipeline for hit sound effects and streaming background music.

---

## Architecture Overview

| File | Responsibility |
| :--- | :--- |
| `Components.h` | Defines pure data structs (`Position`, `Renderable`, `Stats`, `AIController`, `Item`, `RangedAbility`, `FloatingText`, `Stairs`) and type-safe `enum class` states (`GameState`, `AIType`, `ItemType`). |
| `Registry.h` | Manages entity IDs (`uint32_t`), component maps (`std::unordered_map`), non-blocking tile queries (`getEntityAt`), entity destruction (`killEntity`), and floor resets (`clearWorld`). |
| `Systems.h` | Contains all stateless gameplay, pathfinding (BFS), input, targeting, floating text animation, and layered rendering systems. |
| `main.cpp` | Initializes the Raylib window, audio device, and GPU textures; runs the main turn state machine and HUD rendering. |

---

## Controls

| Input | Action |
| :--- | :--- |
| **`W` / `A` / `S` / `D`** | Move one tile orthogonally or bump into an adjacent enemy to perform a melee attack |
| **`Mouse Hover`** | Preview valid target tiles within the 4-tile spell range |
| **`Left Click`** | Cast *Fireball* on a highlighted enemy within range (3-turn cooldown) |

---

## Project Structure

```text
GridTactics/
├── assets/
│   ├── bgm.ogg          # Background music stream (.ogg or .mp3)
│   ├── hit.wav          # Combat hit sound effect (PCM .wav)
│   ├── floor.png        # Dungeon floor tile sprite
│   ├── wall.png         # Dungeon wall tile sprite
│   ├── player.png       # Player character sprite
│   ├── melee_enemy.png  # Melee chaser sprite
│   ├── archer_enemy.png # Ranged archer sprite
│   ├── potion.png       # Health potion sprite
│   ├── boost.png        # Attack boost orb sprite
│   └── stairs.png       # Next-floor stairs sprite
├── Components.h
├── Registry.h
├── Systems.h
├── main.cpp
├── map.txt              # Floor 1 grid layout (20x20)
├── map2.txt             # Floor 2 grid layout (20x20)
├── CMakeLists.txt
└── README.md
```

---

## Build & Run

### Prerequisites
- C++17 compatible compiler (GCC / MinGW / Clang / MSVC)
- [CMake](https://cmake.org/) (3.15+)
- [Raylib](https://www.raylib.com/)

### Compiling with CMake

```bash
# 1. Clone the repository
git clone [https://github.com/your-username/GridTactics.git](https://github.com/your-username/GridTactics.git)
cd GridTactics

# 2. Create a build directory (required so relative ../assets and ../map.txt paths resolve properly)
mkdir build
cd build

# 3. Configure and build
cmake ..
cmake --build .

# 4. Run the executable
./GridTactics        # Linux / macOS
GridTactics.exe      # Windows
```

---

## Customizing Maps & Assets

- **Level Design:** Edit `map.txt` or `map2.txt` using `0` for walkable floor tiles and `1` for solid walls on a 20×20 grid.
- **Sprites & Audio:** Drop any 16×16 or 32×32 `.png` sprites into the `assets/` folder—the renderer automatically scales them to fit grid cells. If any sprite file is missing, the engine seamlessly falls back to color-coded shapes.
