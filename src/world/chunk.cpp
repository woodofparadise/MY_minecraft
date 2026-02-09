#include <glad/glad.h>
#include "chunk.h"

using namespace std;


void Chunk::create_face(Vertex& vertex1, Vertex& vertex2, Vertex& vertex3, Vertex& vertex4)
{
    vertices.push_back(vertex1);
    vertices.push_back(vertex2);
    vertices.push_back(vertex3);
    vertices.push_back(vertex4);
    indices.push_back((unsigned int)vertices.size()-2);
    indices.push_back((unsigned int)vertices.size()-3);
    indices.push_back((unsigned int)vertices.size()-4);
    indices.push_back((unsigned int)vertices.size()-3);
    indices.push_back((unsigned int)vertices.size()-2);
    indices.push_back((unsigned int)vertices.size()-1);
    return ;
}

void Chunk::upload_data()
{
    // 先释放当前对象持有的资源
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);
    if (VBO != 0) glDeleteBuffers(1, &VBO);
    if (EBO != 0) glDeleteBuffers(1, &EBO);

    vertices.shrink_to_fit();
    indices.shrink_to_fit();
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, (size_t)(vertices.size() * sizeof(Vertex)), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Texcoord));
    glEnableVertexAttribArray(0);
}

Chunk::Chunk(PerlinNoise& perlinNoise, int x, int y)
{
    VAO = 0; VBO = 0; EBO = 0;
    isModified = false;
    double step = 1.0f/CHUNK_SIZE;
    chunkBlocks.resize(CHUNK_SIZE);
    heightMap.resize(CHUNK_SIZE);
    
    // 基于二维柏林噪声生成随机地形
    for(int i = 0; i < CHUNK_SIZE; i++)
    {
        chunkBlocks[CHUNK_SIZE-1-i].resize(CHUNK_SIZE);
        heightMap[CHUNK_SIZE-1-i].resize(CHUNK_SIZE);
        for(int j = 0; j < CHUNK_SIZE; j++)
        {
            chunkBlocks[CHUNK_SIZE-1-i][j].resize(CHUNK_HEIGHT, AIR);

            // 使用更平滑的噪声值，范围在 -1 到 1 之间
            double noiseValue = perlinNoise.get_2D_perlin_noice((double)x+step*j, (double)y+step*i);

            // // 计算地形高度，使用更自然的映射，确保有足够的水下地形
            // int height = floor((double)CHUNK_HEIGHT/2 * (noiseValue + 1.0f) * 0.6f + CHUNK_HEIGHT/3.0f);

            // 使用分形噪声+多噪声图混合接近原版效果
            int height = floor(generate_height(perlinNoise, x*CHUNK_SIZE+j, y*CHUNK_SIZE+i));


            height = max(1, min(height, CHUNK_HEIGHT-2)); // 限制高度范围

            // 计算水线高度
            int waterLevel = CHUNK_HEIGHT/2;
            // waterLevel = max(CHUNK_HEIGHT/3, min(waterLevel, CHUNK_HEIGHT*2/3)); // 限制水线范围

            // 如果在水下
            if(height < waterLevel)
            {
                // 海底地形：沙层和石头
                for(int k = 0; k < height-2; k++)
                {
                    chunkBlocks[CHUNK_SIZE-1-i][j][k] = STONE;
                }
                chunkBlocks[CHUNK_SIZE-1-i][j][height-2] = STONE; // 石头层
                chunkBlocks[CHUNK_SIZE-1-i][j][height-1] = SAND;  // 表层沙子

                // 填充水
                for(int k = height; k < waterLevel; k++)
                {
                    chunkBlocks[CHUNK_SIZE-1-i][j][k] = WATER;
                }
            }
            else
            {
                // 陆地地形：更真实的土壤深度和层级
                int soilDepth = max(1, min(5, (int)((noiseValue + 1.0f) * 2.5f))); // 土壤深度基于噪声

                // 基岩层（最底层）
                if(height - soilDepth <= 0)
                {
                    // 如果整个柱体都很浅，全部用石头填充
                    for(int k = 0; k < height - 1; k++)
                    {
                        chunkBlocks[CHUNK_SIZE-1-i][j][k] = STONE;
                    }
                }
                else
                {
                    // 石头层（从底部到土壤层）
                    int stoneEnd = height - soilDepth;
                    for(int k = 0; k < stoneEnd; k++)
                    {
                        chunkBlocks[CHUNK_SIZE-1-i][j][k] = STONE;
                    }
                }

                // 土壤层
                for(int k = height - soilDepth; k < height - 1; k++)
                {
                    chunkBlocks[CHUNK_SIZE-1-i][j][k] = SOIL;
                }

                // 地表层
                chunkBlocks[CHUNK_SIZE-1-i][j][height-1] = GRASS;

                // 如果地形较高，可能有石头露出
                if(height > waterLevel + 32 && rand() % 100 < 20)
                {
                    chunkBlocks[CHUNK_SIZE-1-i][j][height-1] = STONE;
                }
            }

            // cout << height << endl;
            for(int k = 1; k < height; k++)
            {
                double caveNoise = perlinNoise.get_3D_perlin_noice(((double)x+step*j)*3, ((double)y+step*i)*3, (double)k*0.1f);
                if(caveNoise > 0.4f)
                {
                    chunkBlocks[CHUNK_SIZE-1-i][j][k] = AIR;
                }
                else
                {
                    heightMap[CHUNK_SIZE-1-i][j] = k;
                }
            }
            // heightMap[CHUNK_SIZE-1-i][j] = max(heightMap[CHUNK_SIZE-1-i][j], waterLevel);
        }
    }

    // 预计算方块纹理坐标
    sideTexCoords = new glm::vec2[BLOCK_TYPE_NUM];
    topTexCoords = new glm::vec2[BLOCK_TYPE_NUM];
    bottomTexCoords = new glm::vec2[BLOCK_TYPE_NUM];
    for(int blockType = 1; blockType <= BLOCK_TYPE_NUM; ++blockType)
    {
        sideTexCoords[blockType] = get_tex_coord(blockType, 3);
        topTexCoords[blockType] = get_tex_coord(blockType, 1);
        bottomTexCoords[blockType] = get_tex_coord(blockType, 2);
    }

    // 每个方块的索引即为其在该区块中的minCoord
    isModified = true;
}

void Chunk::update_data(const Chunk* left, const Chunk* right, const Chunk* forward, const Chunk* back)
{
    vector<Vertex>().swap(vertices);
    vector<unsigned int>().swap(indices);

    glm::vec2 texRight = glm::vec2(1.0f/16.0f, 0.0f);
    glm::vec2 texDown = glm::vec2(0.0f, -1.0f/16.0f);

    // 只渲染和透明方块接触的表面，其它表面隐藏
    for(int i = CHUNK_SIZE-1; i >= 0; i--)
    {
        for(int j = 0; j < CHUNK_SIZE; j++)
        {
            for(int k = 0; k < CHUNK_HEIGHT; k++)
            {
                BLOCK_TYPE blockType = chunkBlocks[i][j][k];
                if(blockType == AIR)
                {
                    continue;
                }

                glm::vec3 blockPos(j, k, CHUNK_SIZE-1-i);

                glm::vec2 sideTex = sideTexCoords[blockType];
                glm::vec2 topTex = topTexCoords[blockType];
                glm::vec2 bottomTex = bottomTexCoords[blockType];

                // 检查六个方向的邻居，只渲染可见面
                for(int face = 0; face < 6; ++face)
                {
                    BLOCK_TYPE neighborBlock = get_neighbor_block(i, j, k, face, left, right, forward, back);
                    if(!is_transparent(neighborBlock)) continue;              // 邻居不透明，不生成面
                    if(neighborBlock == blockType) continue;                  // 同类型透明方块，不生成面

                    // 选择纹理：face 4=底面, face 5=顶面, 其余=侧面
                    glm::vec2 tex = (face == 5) ? topTex : (face == 4) ? bottomTex : sideTex;

                    Vertex vertex1 = {blockPos + faceVertexOffset[face][0], faceNormal[face], tex};
                    Vertex vertex2 = {blockPos + faceVertexOffset[face][1], faceNormal[face], tex + texRight};
                    Vertex vertex3 = {blockPos + faceVertexOffset[face][2], faceNormal[face], tex + texDown};
                    Vertex vertex4 = {blockPos + faceVertexOffset[face][3], faceNormal[face], tex + texRight + texDown};
                    create_face(vertex1, vertex2, vertex3, vertex4);
                }
            }
        }
    }
    upload_data();
    isModified = false;
}

// 六个面的顶点偏移（相对于方块原点 (xPos, yPos, zPos)）
// 方块占据 [x, x+1] × [y, y+1] × [z, z+1]
const glm::vec3 Chunk::faceVertexOffset[6][4] = {
    // [0] Back (i-1):    z=1 平面（+Z 面）
    {{0,1,1}, {1,1,1}, {0,0,1}, {1,0,1}},
    // [1] Forward (i+1): z=0 平面（-Z 面）
    {{0,1,0}, {1,1,0}, {0,0,0}, {1,0,0}},
    // [2] Left (j-1):    x=0 平面（-X 面）
    {{0,1,1}, {0,1,0}, {0,0,1}, {0,0,0}},
    // [3] Right (j+1):   x=1 平面（+X 面）
    {{1,1,1}, {1,1,0}, {1,0,1}, {1,0,0}},
    // [4] Down (k-1):    y=0 平面（-Y 面）
    {{0,0,1}, {0,0,0}, {1,0,1}, {1,0,0}},
    // [5] Up (k+1):      y=1 平面（+Y 面）
    {{0,1,1}, {0,1,0}, {1,1,1}, {1,1,0}},
};

// 六个面的几何法线（mesh 空间，朝外）
const glm::vec3 Chunk::faceNormal[6] = {
    {0, 0, 1},   // [0] Back:    +Z
    {0, 0,-1},   // [1] Forward: -Z
    {-1, 0, 0},  // [2] Left:    -X
    {1, 0, 0},   // [3] Right:   +X
    {0,-1, 0},   // [4] Down:    -Y
    {0, 1, 0},   // [5] Up:      +Y
};

BLOCK_TYPE Chunk::get_neighbor_block(
    int i, int j, int k,
    int face,
    const Chunk* left, const Chunk* right, const Chunk* forward, const Chunk* back
) const
{
    // 法线 (mesh空间) → 数组偏移: di = -nz (数组i是反向Z), dj = nx, dk = ny
    int ni = i - (int)faceNormal[face].z;
    int nj = j + (int)faceNormal[face].x;
    int nk = k + (int)faceNormal[face].y;

    if(nk >= CHUNK_HEIGHT || nk < 0)
    {
        return AIR;
    }
    if(ni >= 0 && ni < CHUNK_SIZE && nj >= 0 && nj < CHUNK_SIZE)
    {
        return chunkBlocks[ni][nj][nk];
    }
    if(nj == CHUNK_SIZE && right)
    {
        return right->get_block_type(0, CHUNK_SIZE-1-i, k);
    }
    if(nj < 0 && left)
    {
        return left->get_block_type(CHUNK_SIZE-1, CHUNK_SIZE-1-i, k);
    }
    if(ni == CHUNK_SIZE && forward)
    {
        return forward->get_block_type(j, CHUNK_SIZE-1, k);
    }
    if(ni < 0 && back)
    {
        return back->get_block_type(j, 0, k);
    }
    return AIR;
}

bool Chunk::set_block(int i, int j, int k, BLOCK_TYPE blockType)
{
    if(k < 0 || k >= CHUNK_HEIGHT)
    {
        return false;
    }
    isModified = true;
    chunkBlocks[CHUNK_SIZE-1-j][i][k] = blockType;
    return true;
}

// 样条曲线：将 [-1,1] 的 continental 值映射到合理的基础高度
double spline_map_continental(double c)
{
    // c < -0.4: 深海
    // c ∈ [-0.4, -0.1]: 浅海
    // c ∈ [-0.1, 0.1]: 海岸/平原
    // c ∈ [0.1, 0.5]: 丘陵
    // c > 0.5: 山地
    if(c < -0.4) return 30 + (c + 1.0) * 20;      // 深海: 18-42
    if(c < -0.1) return 45 + (c + 0.4) * 40;      // 浅海: 45-57
    if(c < 0.1) return 62 + (c + 0.1) * 30;       // 海岸: 62-68
    if(c < 0.5) return 68 + (c - 0.1) * 50;       // 丘陵: 68-88

    return 88 + (c - 0.5) * 60;                   // 山地: 88-118
}

double Chunk::generate_height(PerlinNoise& perlinNoise, double worldX, double worldZ)
{
    // 使用世界坐标而非区块相对坐标
    // 频率调整为更合理的值

    // 大陆性：控制海洋/陆地，约500-1000格一个周期
    double continental = perlinNoise.get_fbm_noise(
        worldX * 0.005, worldZ * 0.005, 4, 0.5, 2.0);

    // 侵蚀度：控制平原/山地，约100-200格一个周期
    double erosion = perlinNoise.get_fbm_noise(
        worldX * 0.008 + 1000.0f, worldZ * 0.008 + 1000.0f, 4, 0.5, 2.0);

    // 峰谷：局部起伏，约20-50格一个周期
    double peaks = perlinNoise.get_fbm_noise(
        worldX * 0.02 + 2000.0f, worldZ * 0.02 + 2000.0f, 6, 0.5, 2.0);

    // 使用样条曲线映射 continental 值
    double continentHeight = spline_map_continental(continental);

    // erosion 控制 peaks 的影响程度（高侵蚀=更平坦）
    double erosionFactor = 1.0 - (erosion + 1.0) * 0.4;  // [0.2, 1.0]
    erosionFactor = max(0.1, erosionFactor);

    double peakHeight = peaks * 25 * erosionFactor;

    return continentHeight + peakHeight;
}

Chunk::Chunk(Chunk&& other) noexcept
      : chunkBlocks(std::move(other.chunkBlocks)),
        heightMap(std::move(other.heightMap)),
        vertices(std::move(other.vertices)),
        indices(std::move(other.indices)),
        VAO(other.VAO),
        VBO(other.VBO),
        EBO(other.EBO),
        isModified(other.isModified)
  {
      other.VAO = 0;
      other.VBO = 0;
      other.EBO = 0;
      other.isModified = false;
  }

Chunk& Chunk::operator=(Chunk&& other) noexcept
{
    if (this != &other)  // 防止自赋值
    {
        // 先释放当前对象持有的资源
        if (VAO != 0) glDeleteVertexArrays(1, &VAO);
        if (VBO != 0) glDeleteBuffers(1, &VBO);
        if (EBO != 0) glDeleteBuffers(1, &EBO);

        // 窃取源对象资源
        chunkBlocks = std::move(other.chunkBlocks);
        heightMap = std::move(other.heightMap);
        vertices = std::move(other.vertices);
        indices = std::move(other.indices);
        VAO = other.VAO;
        VBO = other.VBO;
        EBO = other.EBO;
        isModified = other.isModified;

        // 源对象置空
        other.VAO = 0;
        other.VBO = 0;
        other.EBO = 0;
        other.isModified = false;
    }
    return *this;
}
