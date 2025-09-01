# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a homemade Minecraft clone built with C++ and OpenGL. The project implements a voxel-based 3D world with basic Minecraft-like functionality including block placement, destruction, terrain generation, and player movement.

## Build and Development Commands

Since no build system files (Makefile, CMakeLists.txt) are present, manual compilation is required:

```bash
# Compile the project (adjust library paths as needed for your system)
g++ -o main main.cpp glad.c -lglfw -lGL -ldl

# Run the game
./main
```

**Note**: You'll need the following dependencies installed:
- OpenGL development libraries
- GLFW3
- GLM (header-only library)
- GLAD (already included as glad.c)

## Architecture Overview

### Core Game Structure

The main game logic is encapsulated in a `Game` class (main.cpp:18) that manages:
- Player state and movement
- Terrain generation and rendering
- Input handling and game loop
- Shader management for rendering and block selection

### Key Components

**Terrain System** (terrain.h):
- Uses chunk-based world generation with Perlin noise
- Manages a map of chunks using `std::map<std::pair<int, int>, std::unique_ptr<Chunk>>`
- Each chunk is 64x128x64 blocks (chunkSize × chunkHeight × chunkSize)

**Chunk System** (chunk.h):
- Generates terrain using Perlin noise for height mapping
- Optimizes rendering by only generating faces for visible block sides
- Manages vertex buffers (VAO, VBO, EBO) for efficient OpenGL rendering

**Block Types** (block.h):
- Enum-based block system: AIR, GRASS, STONE, SAND, WATER, SOIL
- Simple struct-based block representation

**Player System** (player.h):
- Handles player movement, collision detection, and camera control
- Includes functions for creating player geometry (cuboid generation)

**Rendering Pipeline**:
- Two shader programs: `blockShader` (world rendering) and `selectionShader` (block highlighting)
- Texture atlas system for block textures
- Camera system with first-person perspective

### Important File Locations

- Main game loop: `main.cpp`
- Shader files: `shaders/blockShader.vs`, `shaders/blockShader.fs`, `shaders/selectionShader.vs`, `shaders/selectionShader.fs`
- Block textures: `blocks/` directory (contains hundreds of Minecraft-style textures)
- Core headers: `*.h` files in root directory

### Development Notes

- The project uses modern OpenGL with vertex arrays and shaders
- Terrain generation uses 2D Perlin noise for realistic world generation
- Block selection is implemented with ray casting and highlighting
- The codebase includes Chinese comments indicating this is an educational project
- Memory management uses smart pointers for chunk handling