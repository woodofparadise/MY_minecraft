# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

A Minecraft clone built with C++ and OpenGL 4.5, featuring voxel-based terrain with Perlin noise generation, block placement/destruction, physics-based player movement, and first/third-person camera modes.

## Build Commands

```bash
# Compile (adjust library paths for your system)
g++ -o main main.cpp lib/glad/glad.c -lglfw -lGL -ldl

# Run
./main
```

**Dependencies**: OpenGL dev libraries, GLFW3, GLM (header-only), GLAD (included)

## Game Controls

- **WASD**: Movement
- **Mouse**: Look around
- **Space**: Jump
- **R**: Toggle first/third person view
- **Left Click**: Place block
- **Right Click**: Destroy block
- **1-9**: Select toolbar slot
- **Scroll**: Zoom
- **ESC**: Quit

## Project Structure

```
minecraft/
├── main.cpp                 # 程序入口
├── src/
│   ├── core/                # 核心系统
│   │   ├── game.h           # 游戏主类
│   │   ├── camera.h         # 摄像机
│   │   └── preDefined.h     # 全局常量
│   ├── world/               # 世界/地形系统
│   │   ├── terrain.h        # 地形管理
│   │   ├── chunk.h          # 区块
│   │   ├── block.h          # 方块类型
│   │   └── perlin_noise.h   # 柏林噪声
│   ├── entity/              # 实体系统
│   │   ├── player.h         # 玩家
│   │   └── collision.h      # 碰撞检测
│   ├── render/              # 渲染系统
│   │   ├── Shader.h         # 着色器封装
│   │   ├── texture.h        # 纹理加载
│   │   └── basic_struct.h   # 顶点结构
│   ├── ui/                  # 用户界面
│   │   ├── HUDpainter.h     # HUD绘制
│   │   ├── toolBar.h        # 工具栏
│   │   └── itemSelection.h  # 方块选择
│   └── utils/               # 工具库
│       └── stb_image.h      # 图像加载
├── lib/glad/                # GLAD库
├── shaders/                 # 着色器文件
└── Textures/                # 纹理资源
```

## Architecture

### Core Flow
`main.cpp` → `Game` class (src/core/game.h) → game loop managing:
- Input callbacks via GLFW (`glfwSetWindowUserPointer` pattern for static callbacks)
- Per-frame updates: player physics, terrain loading, rendering

### Rendering Pipeline
Three shader programs in `shaders/`:
- `blockShader`: World terrain with texture atlas (DefaultPack.png - 16x16 grid)
- `selectionShader`: Wireframe for selected block highlight
- `HUDShader`: 2D overlay elements (toolbar, cursor) with orthographic projection

Texture binding slots: 1=blocks, 2=player, 3=cursor, 4=toolbar

### Terrain System (src/world/)
- Chunks: 64×128×64 blocks (chunkSize × chunkHeight × chunkSize)
- Storage: `std::map<std::pair<int, int>, std::unique_ptr<Chunk>>` keyed by (x,z) indices
- Generation: 2D Perlin noise determines height; layers assigned by depth
- Optimization: Only faces adjacent to AIR blocks are meshed (update_data checks 4 neighbors)
- Modification: `isModified` flag triggers remeshing on next update

### Block System (src/world/block.h)
Block types: AIR(0), GRASS, STONE, SAND, WATER, SOIL, WOOD, GLASS, COAL, IRON, GOLD, DIAMOND
`get_tex_coord(blockType, face)` maps to texture atlas positions (face: 1=top, 2=bottom, 3=side)

### Player & Physics (src/entity/)
- AABB collision detection against terrain blocks
- Gravity: -9.8/10 units/s², jump force: 0.1 units
- Player model: Steve-style with textured body parts (64x64 skin texture)
- Camera modes: first-person (inside head) and third-person (offset behind)

### Ray Casting (src/ui/itemSelection.h)
`raycast_step()`: Integer grid stepping algorithm for block selection
- Max range: 4.0 units
- Returns both hit block and last air block (for placement)

### HUD System (src/ui/)
- HUDitem: Positioned quads with orthographic rendering
- toolBar: 9-slot hotbar using widgets.png texture regions

## Key Coordinate Notes
- Y is vertical (height), chunk z-index has +1 offset correction in draw_terrain
- World origin at chunk center: blocks offset by chunkSize/2
- Player position is at feet center; camera offset varies by view mode
