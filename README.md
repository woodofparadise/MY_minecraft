# Homemade Minecraft

基于 C++ 和 OpenGL 4.5 的 Minecraft 克隆项目，用于学习图形学和游戏开发。

## 功能特性

- **程序化地形生成**：使用多层柏林噪声（FBM）和样条曲线映射生成丰富地形，包含深海、浅海、海岸、平原、丘陵、山地等多种地貌
- **方块系统**：支持 12 种方块类型（草方块、石头、沙子、水、泥土、木头、玻璃、煤矿、铁矿、金矿、钻石）
- **方块交互**：射线检测实现方块选中高亮，支持破坏和放置方块
- **物理系统**：重力、跳跃、AABB 碰撞检测
- **玩家模型**：Steve 风格的可渲染玩家角色，支持皮肤纹理
- **视角切换**：第一人称/第三人称视角切换
- **HUD 界面**：屏幕准星、9 格工具栏、方块选择

## 构建与运行

### 依赖项

- OpenGL 4.5+
- GLFW3
- GLM (header-only)
- GLAD (已包含)

### 编译

```bash
g++ -o main main.cpp lib/glad/glad.c -lglfw -lGL -ldl
```

### 运行

```bash
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
├── main.cpp              # 程序入口
├── src/
│   ├── core/             # 游戏核心 (Game, Camera)
│   ├── world/            # 世界系统 (Terrain, Chunk, Block, PerlinNoise)
│   ├── entity/           # 实体系统 (Player, Collision)
│   ├── render/           # 渲染系统 (Shader, Texture)
│   ├── ui/               # 用户界面 (HUD, ToolBar, ItemSelection)
│   └── utils/            # 工具库 (stb_image)
├── lib/glad/             # GLAD 库
├── shaders/              # GLSL 着色器
└── Textures/             # 纹理资源
```

## 技术实现

- **区块系统**：64×128×64 的区块划分，动态加载玩家周围 3×3 区块
- **地形生成**：
  - 分形布朗运动噪声（FBM）叠加多层 Perlin 噪声
  - 三层噪声混合：大陆性（continental）、侵蚀度（erosion）、峰谷（peaks）
  - 样条曲线映射实现平滑的海洋-平原-山地过渡
- **渲染优化**：仅渲染与空气接触的方块表面，相邻区块边界面剔除
- **纹理图集**：16×16 方块纹理图集，支持顶面/底面/侧面不同纹理
- **着色器**：方块渲染、选中高亮、HUD 三套独立着色器

## 许可证

无，just for fun.
