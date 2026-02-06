# Homemade Minecraft

基于 C++ 和 OpenGL 4.5 的 Minecraft 克隆项目，用于学习图形学和游戏开发。

## 功能特性

- **程序化地形生成**：使用多层柏林噪声（FBM）和样条曲线映射生成丰富地形，包含深海、浅海、海岸、平原、丘陵、山地等多种地貌
- **方块系统**：支持 12 种方块类型（草方块、石头、沙子、水、泥土、木头、玻璃、煤矿、铁矿、金矿、钻石）
- **方块交互**：射线检测实现方块选中高亮，支持破坏和放置方块
- **物理系统**：重力、跳跃、AABB 碰撞检测
- **玩家模型**：Steve 风格的可渲染玩家角色，支持皮肤纹理
- **视角切换**：第一人称/第三人称视角切换
- **HUD 界面**：屏幕准星、9 格工具栏、方块选择、FPS 显示

## 构建与运行

### 依赖项

- OpenGL 4.5+
- GLFW3
- GLM (header-only)
- FreeType2 (字体渲染)
- GLAD (已包含)

### 安装依赖 (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install -y libglfw3-dev libglm-dev libfreetype-dev
```

### 编译方法一：CMake（推荐）

```bash
# 创建构建目录
mkdir build && cd build

# 配置项目
cmake ..

# 编译（使用所有 CPU 核心）
cmake --build . -j$(nproc)

# 或者使用 make
make -j$(nproc)
```

### 编译方法二：直接编译

```bash
g++ -o main main.cpp \
    src/core/game.cpp src/core/camera.cpp \
    src/world/chunk.cpp src/world/terrain.cpp src/world/block.cpp \
    src/entity/player.cpp src/entity/collision.cpp \
    src/render/texture.cpp \
    src/ui/HUDpainter.cpp src/ui/toolBar.cpp src/ui/itemSelection.cpp \
    src/utils/stb_image.cpp \
    lib/glad/glad.c \
    -I./include -lglfw -lGL -ldl -lfreetype
```

### 运行

```bash
# 如果使用 CMake 构建
cd build
./MyMinecraft

# 如果使用直接编译
./main
```

## 操作说明

| 按键 | 功能 |
|------|------|
| W/A/S/D | 移动 |
| 鼠标 | 视角控制 |
| 空格 | 跳跃 |
| R | 切换第一/第三人称 |
| 鼠标左键 | 放置方块 |
| 鼠标右键 | 破坏方块 |
| 1-9 | 选择工具栏槽位 |
| 滚轮 | 缩放视野 |
| ESC | 退出游戏 |

## 项目结构

```
minecraft/
├── main.cpp                    # 程序入口
├── CMakeLists.txt              # CMake 配置文件
├── src/
│   ├── core/                   # 游戏核心
│   │   ├── game.h / game.cpp           # 游戏主类
│   │   ├── camera.h / camera.cpp       # 摄像机
│   │   └── preDefined.h                # 全局常量
│   ├── world/                  # 世界系统
│   │   ├── terrain.h / terrain.cpp     # 地形管理
│   │   ├── chunk.h / chunk.cpp         # 区块
│   │   ├── block.h / block.cpp         # 方块类型
│   │   └── perlin_noise.h              # 柏林噪声
│   ├── entity/                 # 实体系统
│   │   ├── player.h / player.cpp       # 玩家
│   │   └── collision.h / collision.cpp # 碰撞检测
│   ├── render/                 # 渲染系统
│   │   ├── Shader.h                    # 着色器封装
│   │   ├── texture.h / texture.cpp     # 纹理加载
│   │   └── basic_struct.h              # 顶点结构
│   ├── ui/                     # 用户界面
│   │   ├── HUDpainter.h / HUDpainter.cpp   # HUD 绘制
│   │   ├── toolBar.h / toolBar.cpp         # 工具栏
│   │   ├── itemSelection.h / itemSelection.cpp # 方块选择
│   │   └── textRenderer.h              # 文字渲染
│   └── utils/                  # 工具库
│       ├── stb_image.h                 # 图像加载（头文件）
│       └── stb_image.cpp               # 图像加载（实现）
├── lib/glad/                   # GLAD 库
├── shaders/                    # GLSL 着色器
│   ├── blockShader.vs / .fs            # 方块渲染
│   ├── selectionShader.vs / .fs        # 选中高亮
│   ├── HUDshader.vs / .fs              # HUD 渲染
│   └── textShader.vs / .fs             # 文字渲染
└── Textures/                   # 纹理资源
```

## 技术实现

- **区块系统**：64×128×64 的区块划分，动态加载玩家周围 3×3 区块
- **地形生成**：
  - 分形布朗运动噪声（FBM）叠加多层 Perlin 噪声
  - 三层噪声混合：大陆性（continental）、侵蚀度（erosion）、峰谷（peaks）
  - 样条曲线映射实现平滑的海洋-平原-山地过渡
- **渲染优化**：仅渲染与空气接触的方块表面，相邻区块边界面剔除
- **纹理图集**：16×16 方块纹理图集，支持顶面/底面/侧面不同纹理
- **着色器**：方块渲染、选中高亮、HUD、文字渲染四套独立着色器

## 许可证

无，just for fun.
