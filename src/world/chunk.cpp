#include <glad/glad.h>
#include "chunk.h"
#include <algorithm>

using namespace std;

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

const glm::ivec3 Chunk::arrayOffset[6] = {
      { 1,  0,  0},  // i+1 (数组Z方向+)
      {-1,  0,  0},  // i-1 (数组Z方向-)
      { 0,  1,  0},  // j+1 (数组X方向+)
      { 0, -1,  0},  // j-1 (数组X方向-)
      { 0,  0,  1},  // k+1 (数组Y方向+，向上)
      { 0,  0, -1},  // k-1 (数组Y方向-，向下)
  };

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

void Chunk::create_face_transparent(Vertex& vertex1, Vertex& vertex2, Vertex& vertex3, Vertex& vertex4)
{
    verticesT.push_back(vertex1);
    verticesT.push_back(vertex2);
    verticesT.push_back(vertex3);
    verticesT.push_back(vertex4);
    indicesT.push_back((unsigned int)verticesT.size()-2);
    indicesT.push_back((unsigned int)verticesT.size()-3);
    indicesT.push_back((unsigned int)verticesT.size()-4);
    indicesT.push_back((unsigned int)verticesT.size()-3);
    indicesT.push_back((unsigned int)verticesT.size()-2);
    indicesT.push_back((unsigned int)verticesT.size()-1);
    // 记录面片中心用于每帧透明排序
    transparentFaceCenters.push_back(
        (vertex1.Position + vertex2.Position + vertex3.Position + vertex4.Position) * 0.25f);
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
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, LightLevel));
}

void Chunk::upload_data_transparent()
{
    // 先释放当前对象持有的资源
    if (transparentVAO != 0) glDeleteVertexArrays(1, &transparentVAO);
    if (transparentVBO != 0) glDeleteBuffers(1, &transparentVBO);
    if (transparentEBO != 0) glDeleteBuffers(1, &transparentEBO);

    verticesT.shrink_to_fit();
    indicesT.shrink_to_fit();
    glGenVertexArrays(1, &transparentVAO);
    glBindVertexArray(transparentVAO);
    glGenBuffers(1, &transparentVBO);

    glGenBuffers(1, &transparentEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, transparentEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesT.size() * sizeof(unsigned int), &indicesT[0], GL_DYNAMIC_DRAW);


    glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
    glBufferData(GL_ARRAY_BUFFER, (size_t)(verticesT.size() * sizeof(Vertex)), &verticesT[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Texcoord));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, LightLevel));
}

Chunk::Chunk(PerlinNoise& perlinNoise, int x, int y)
{
    VAO = 0; VBO = 0; EBO = 0;
    transparentVAO = 0; transparentVBO = 0; transparentEBO = 0;
    isModified = false;
    double step = 1.0f/CHUNK_SIZE;
    chunkBlocks.resize(CHUNK_SIZE);
    heightMap.resize(CHUNK_SIZE);
    blockLights.resize(CHUNK_SIZE,
        std::vector<std::vector<short>>(CHUNK_SIZE,
            std::vector<short>(CHUNK_HEIGHT, 0)));
    
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

    init_local_light();

    // 每个方块的索引即为其在该区块中的minCoord
    isModified = true;
}

void Chunk::update_data(const Chunk* neighbours[4])
{
    vector<Vertex>().swap(vertices);
    vector<unsigned int>().swap(indices);
    vector<Vertex>().swap(verticesT);
    vector<unsigned int>().swap(indicesT);
    vector<glm::vec3>().swap(transparentFaceCenters);

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
                    BLOCK_TYPE neighborBlock = get_neighbor_block(i, j, k, face, neighbours);
                    if(!is_transparent(neighborBlock))
                    {
                        continue;              // 邻居不透明，不生成面
                    } 
                    if(neighborBlock == blockType)
                    {
                        continue;                  // 同类型透明方块，不生成面
                    }

                    // 选择纹理：face 4=底面, face 5=顶面, 其余=侧面
                    glm::vec2 tex = (face == 5) ? topTex : (face == 4) ? bottomTex : sideTex;

                    // 该面的光照等级 = 相邻透明方块的光照值
                    float light = get_neighbor_light(i, j, k, face, neighbours);

                    Vertex vertex1 = {blockPos + faceVertexOffset[face][0], faceNormal[face], tex, light};
                    Vertex vertex2 = {blockPos + faceVertexOffset[face][1], faceNormal[face], tex + texRight, light};
                    Vertex vertex3 = {blockPos + faceVertexOffset[face][2], faceNormal[face], tex + texDown, light};
                    Vertex vertex4 = {blockPos + faceVertexOffset[face][3], faceNormal[face], tex + texRight + texDown, light};
                    if(is_transparent(blockType))
                    {
                        create_face_transparent(vertex1, vertex2, vertex3, vertex4);
                    }
                    else
                    {
                        create_face(vertex1, vertex2, vertex3, vertex4);
                    }
                }
            }
        }
    }
    upload_data();
    upload_data_transparent();
    isModified = false;
}

BLOCK_TYPE Chunk::get_neighbor_block(
    int i, int j, int k,
    int face,
    const Chunk* neighbours[4]
) const
{
    // neighbours: [0]=left(-X), [1]=right(+X), [2]=forward(-Z), [3]=back(+Z)
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
    if(nj == CHUNK_SIZE && neighbours[1])
    {
        return neighbours[1]->get_block_type(0, CHUNK_SIZE-1-i, k);
    }
    if(nj < 0 && neighbours[0])
    {
        return neighbours[0]->get_block_type(CHUNK_SIZE-1, CHUNK_SIZE-1-i, k);
    }
    if(ni == CHUNK_SIZE && neighbours[2])
    {
        return neighbours[2]->get_block_type(j, CHUNK_SIZE-1, k);
    }
    if(ni < 0 && neighbours[3])
    {
        return neighbours[3]->get_block_type(j, 0, k);
    }
    return AIR;
}

float Chunk::get_neighbor_light(
    int i, int j, int k,
    int face,
    const Chunk* neighbours[4]
) const
{
    // neighbours: [0]=left(-X), [1]=right(+X), [2]=forward(-Z), [3]=back(+Z)
    int ni = i - (int)faceNormal[face].z;
    int nj = j + (int)faceNormal[face].x;
    int nk = k + (int)faceNormal[face].y;

    if(nk >= CHUNK_HEIGHT) return 15.0f;  // 世界顶部以上，天空满亮度
    if(nk < 0) return 0.0f;               // 世界底部以下，全黑

    if(ni >= 0 && ni < CHUNK_SIZE && nj >= 0 && nj < CHUNK_SIZE)
    {
        return (float)blockLights[ni][nj][nk];
    }
    // 跨区块访问：get_block_light 使用数组索引（无翻转）
    if(nj == CHUNK_SIZE && neighbours[1])
        return (float)neighbours[1]->get_block_light({i, 0, nk});
    if(nj < 0 && neighbours[0])
        return (float)neighbours[0]->get_block_light({i, CHUNK_SIZE-1, nk});
    if(ni == CHUNK_SIZE && neighbours[2])
        return (float)neighbours[2]->get_block_light({0, j, nk});
    if(ni < 0 && neighbours[3])
        return (float)neighbours[3]->get_block_light({CHUNK_SIZE-1, j, nk});

    return 15.0f;  // 邻居区块未加载，默认满亮度
}

void Chunk::sort_transparent_faces(const glm::vec3& localCameraPos)
{
    int faceCount = (int)transparentFaceCenters.size();
    if(faceCount <= 1) return;

    // 创建面片索引数组 [0, 1, ..., N-1]
    vector<int> faceOrder(faceCount);
    for(int i = 0; i < faceCount; i++) faceOrder[i] = i;

    // 按距离摄像机从远到近排序
    sort(faceOrder.begin(), faceOrder.end(), [&](int a, int b) {
        glm::vec3 da = transparentFaceCenters[a] - localCameraPos;
        glm::vec3 db = transparentFaceCenters[b] - localCameraPos;
        return glm::dot(da, da) > glm::dot(db, db);
    });

    // 按排序顺序重建 indicesT（每个面片固定模式: base+2, base+1, base+0, base+1, base+2, base+3）
    for(int i = 0; i < faceCount; i++)
    {
        unsigned int base = faceOrder[i] * 4;
        indicesT[i*6+0] = base + 2;
        indicesT[i*6+1] = base + 1;
        indicesT[i*6+2] = base + 0;
        indicesT[i*6+3] = base + 1;
        indicesT[i*6+4] = base + 2;
        indicesT[i*6+5] = base + 3;
    }

    // 仅重传透明EBO（VBO不动）
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, transparentEBO);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indicesT.size() * sizeof(unsigned int), indicesT.data());
}

bool Chunk::set_block(int x, int y, int z, BLOCK_TYPE blockType, Chunk* neighbours[4])
{
    if(z < 0 || z >= CHUNK_HEIGHT)
    {
        return false;
    }
    isModified = true;
    BLOCK_TYPE oldType = chunkBlocks[CHUNK_SIZE-1-y][x][z];
    chunkBlocks[CHUNK_SIZE-1-y][x][z] = blockType;
    // 数组索引: i = CHUNK_SIZE-1-y, j = x, k = z
    if(blockType == AIR && oldType != AIR)
    {
        update_light_on_destroy({CHUNK_SIZE-1-y, x, z}, neighbours);
    }
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
        blockLights(std::move(other.blockLights)),
        vertices(std::move(other.vertices)),
        verticesT(std::move(other.verticesT)),
        transparentFaceCenters(std::move(other.transparentFaceCenters)),
        indices(std::move(other.indices)),
        indicesT(std::move(other.indicesT)),
        VAO(other.VAO),
        VBO(other.VBO),
        EBO(other.EBO),
        transparentVAO(other.transparentVAO),
        transparentVBO(other.transparentVBO),
        transparentEBO(other.transparentEBO),
        isModified(other.isModified),
        isLightDirty(other.isLightDirty)
  {
      other.VAO = 0;
      other.VBO = 0;
      other.EBO = 0;
      other.transparentVAO = 0;
      other.transparentVBO = 0;
      other.transparentEBO = 0;
      other.isModified = false;
      other.isLightDirty = false;
  }

Chunk& Chunk::operator=(Chunk&& other) noexcept
{
    if (this != &other)  // 防止自赋值
    {
        // 先释放当前对象持有的资源
        if (VAO != 0) glDeleteVertexArrays(1, &VAO);
        if (VBO != 0) glDeleteBuffers(1, &VBO);
        if (EBO != 0) glDeleteBuffers(1, &EBO);
        if (transparentVAO != 0) glDeleteVertexArrays(1, &transparentVAO);
        if (transparentVBO != 0) glDeleteBuffers(1, &transparentVBO);
        if (transparentEBO != 0) glDeleteBuffers(1, &transparentEBO);

        // 窃取源对象资源
        chunkBlocks = std::move(other.chunkBlocks);
        heightMap = std::move(other.heightMap);
        blockLights = std::move(other.blockLights);
        vertices = std::move(other.vertices);
        verticesT = std::move(other.verticesT);
        transparentFaceCenters = std::move(other.transparentFaceCenters);
        indices = std::move(other.indices);
        indicesT = std::move(other.indicesT);
        VAO = other.VAO;
        VBO = other.VBO;
        EBO = other.EBO;
        transparentVAO = other.transparentVAO;
        transparentVBO = other.transparentVBO;
        transparentEBO = other.transparentEBO;
        isModified = other.isModified;
        isLightDirty = other.isLightDirty;

        // 源对象置空
        other.VAO = 0;
        other.VBO = 0;
        other.EBO = 0;
        other.transparentVAO = 0;
        other.transparentVBO = 0;
        other.transparentEBO = 0;
        other.isModified = false;
        other.isLightDirty = false;
    }
    return *this;
}

bool Chunk::is_valid_index(const glm::ivec3& index)
{
    if(index.x < 0 || index.x >= CHUNK_SIZE || index.y < 0 || index.y >= CHUNK_SIZE || index.z < 0 || index.z >= CHUNK_HEIGHT)
    {
        return false;
    }
    return true;
}

short Chunk::get_block_light(const glm::ivec3& index) const
{
    if(!is_valid_index(index))
    {
        return 0;
    }
    return blockLights[index.x][index.y][index.z];
}

void Chunk::init_local_light()
{
    std::queue<glm::ivec3 > lightBFS;
    for(int i = 0; i < CHUNK_SIZE; ++i)
    {
        for(int j = 0; j < CHUNK_SIZE; ++j)
        {
            for(int k = CHUNK_HEIGHT-1; k >= 0; k--)
            {
                if(chunkBlocks[i][j][k] != AIR)
                {
                    break;
                }
                blockLights[i][j][k] = 15;
                lightBFS.push(glm::ivec3(i, j, k));
            }
        }
    }
    update_block_light(lightBFS);
    return ;
}

void Chunk::update_block_light(std::queue<glm::ivec3 >& lightBFS)
{
    while(!lightBFS.empty())
    {
        glm::ivec3 local = lightBFS.front();
        lightBFS.pop();
        for(int i = 0; i < 6; ++i)
        {
            glm::ivec3 temp = local + arrayOffset[i];
            if(!is_valid_index(temp))
            {
                continue;
            }
            else if(blockLights[temp.x][temp.y][temp.z] >= blockLights[local.x][local.y][local.z])
            {
                continue;
            }
            int dec = get_opacity(chunkBlocks[temp.x][temp.y][temp.z]);
            if(blockLights[local.x][local.y][local.z] - dec <= 0)
            {
                blockLights[temp.x][temp.y][temp.z] = 0;
                continue;
            }
            blockLights[temp.x][temp.y][temp.z] = blockLights[local.x][local.y][local.z] - dec;
            lightBFS.push(temp);
        }
    }
}

void Chunk::update_chunk_light(const Chunk* neighbours[4])
{
    // neighbours: [0]=left(-X), [1]=right(+X), [2]=forward(-Z), [3]=back(+Z)
    // 数组维度: i→Z反向, j→X, k→Y
    // j=0 侧邻居=left([0]), j=max 侧邻居=right([1])
    // i=0 侧邻居=back([3]), i=max 侧邻居=forward([2])
    std::queue<glm::ivec3> lightBFS;

    // j 方向边界: left / right
    for(int i = 0; i < CHUNK_SIZE; ++i)
    {
        for(int k = 0; k < CHUNK_HEIGHT; ++k)
        {
            // j=0 侧 ← left
            int newLight = neighbours[0]->get_block_light({i, CHUNK_SIZE-1, k}) - get_opacity(chunkBlocks[i][0][k]);
            if(newLight > blockLights[i][0][k])
            {
                blockLights[i][0][k] = newLight;
                lightBFS.push({i, 0, k});
            }
            // j=max 侧 ← right
            newLight = neighbours[1]->get_block_light({i, 0, k}) - get_opacity(chunkBlocks[i][CHUNK_SIZE-1][k]);
            if(newLight > blockLights[i][CHUNK_SIZE-1][k])
            {
                blockLights[i][CHUNK_SIZE-1][k] = newLight;
                lightBFS.push({i, CHUNK_SIZE-1, k});
            }
        }
    }

    // i 方向边界: forward / back
    for(int j = 0; j < CHUNK_SIZE; ++j)
    {
        for(int k = 0; k < CHUNK_HEIGHT; ++k)
        {
            // i=max 侧 ← forward
            int newLight = neighbours[2]->get_block_light({0, j, k}) - get_opacity(chunkBlocks[CHUNK_SIZE-1][j][k]);
            if(newLight > blockLights[CHUNK_SIZE-1][j][k])
            {
                blockLights[CHUNK_SIZE-1][j][k] = newLight;
                lightBFS.push({CHUNK_SIZE-1, j, k});
            }
            // i=0 侧 ← back
            newLight = neighbours[3]->get_block_light({CHUNK_SIZE-1, j, k}) - get_opacity(chunkBlocks[0][j][k]);
            if(newLight > blockLights[0][j][k])
            {
                blockLights[0][j][k] = newLight;
                lightBFS.push({0, j, k});
            }
        }
    }
    update_block_light(lightBFS);
}

void Chunk::update_light_on_destroy(const glm::ivec3& index, Chunk* neighbours[4])
{
    // Step 1: 天空光柱恢复检查
    bool isSunLight = true;
    std::queue<glm::ivec3> lightBFS;
    for(int k = index.z; k < CHUNK_HEIGHT; ++k)
    {
        if(chunkBlocks[index.x][index.y][k] != AIR)
        {
            isSunLight = false;
            break;
        }
    }
    if(isSunLight)
    {
        // 该位置暴露在天空下，向下设置满亮度直到遇到非透明方块
        for(int k = index.z; k >= 0; --k)
        {
            if(chunkBlocks[index.x][index.y][k] != AIR)
            {
                break;
            }
            blockLights[index.x][index.y][k] = 15;
            lightBFS.push({index.x, index.y, k});
        }
        update_block_light(lightBFS);
    }
    else
    {
        // Step 2: 非天空柱——从6个邻居中采集最大光照
        blockLights[index.x][index.y][index.z] = 0;
        short dec = (short)get_opacity(chunkBlocks[index.x][index.y][index.z]);
        for(int i = 0; i < 6; i++)
        {
            glm::ivec3 temp = index + arrayOffset[i];
            if(is_valid_index(temp))
            {
                short candidate = blockLights[temp.x][temp.y][temp.z] - dec;
                if(candidate > blockLights[index.x][index.y][index.z])
                    blockLights[index.x][index.y][index.z] = candidate;
            }
            else
            {
                // 跨区块邻居采光
                short nbLight = 0;
                if(temp.z >= CHUNK_HEIGHT)
                    nbLight = 15;
                else if(temp.z < 0)
                    nbLight = 0;
                else if(temp.y >= CHUNK_SIZE && neighbours[1])
                    nbLight = neighbours[1]->get_block_light({temp.x, 0, temp.z});
                else if(temp.y < 0 && neighbours[0])
                    nbLight = neighbours[0]->get_block_light({temp.x, CHUNK_SIZE-1, temp.z});
                else if(temp.x >= CHUNK_SIZE && neighbours[2])
                    nbLight = neighbours[2]->get_block_light({0, temp.y, temp.z});
                else if(temp.x < 0 && neighbours[3])
                    nbLight = neighbours[3]->get_block_light({CHUNK_SIZE-1, temp.y, temp.z});
                short candidate = nbLight - dec;
                if(candidate > blockLights[index.x][index.y][index.z])
                    blockLights[index.x][index.y][index.z] = candidate;
            }
        }

        // Step 3: 正向BFS传播
        lightBFS.push(index);
        update_block_light(lightBFS);
    }

    // Step 4: 根据BFS最大传播距离(15)标记可能受影响的邻居区块
    // BFS从 index 出发最远传播15格，若能到达某个边界则标记该邻居
    // neighbours: [0]=left(-X,j=0), [1]=right(+X,j=max), [2]=forward(-Z,i=max), [3]=back(+Z,i=0)
    if(index.y < 15 && neighbours[0])              neighbours[0]->isLightDirty = true;
    if(index.y >= CHUNK_SIZE - 15 && neighbours[1]) neighbours[1]->isLightDirty = true;
    if(index.x >= CHUNK_SIZE - 15 && neighbours[2]) neighbours[2]->isLightDirty = true;
    if(index.x < 15 && neighbours[3])              neighbours[3]->isLightDirty = true;
}

int Chunk::normal_to_face(const glm::vec3& normal)
{
    if(normal.z > 0.5f)  return 0;  // Back:    +Z
    if(normal.z < -0.5f) return 1;  // Forward: -Z
    if(normal.x < -0.5f) return 2;  // Left:    -X
    if(normal.x > 0.5f)  return 3;  // Right:   +X
    if(normal.y < -0.5f) return 4;  // Down:    -Y
    return 5;                        // Up:      +Y
}

void Chunk::refresh_vertex_lights(const Chunk* neighbours[4])
{
    // 每4个顶点组成一个面片，共享同一方块和面方向
    for(size_t v = 0; v + 3 < vertices.size(); v += 4)
    {
        int face = normal_to_face(vertices[v].Normal);
        // 从顶点位置反推方块的 mesh 局部坐标
        glm::vec3 blockPos = vertices[v].Position - faceVertexOffset[face][0];
        int i = CHUNK_SIZE - 1 - (int)blockPos.z;
        int j = (int)blockPos.x;
        int k = (int)blockPos.y;
        float light = get_neighbor_light(i, j, k, face, neighbours);
        vertices[v].LightLevel = light;
        vertices[v+1].LightLevel = light;
        vertices[v+2].LightLevel = light;
        vertices[v+3].LightLevel = light;
    }
    for(size_t v = 0; v + 3 < verticesT.size(); v += 4)
    {
        int face = normal_to_face(verticesT[v].Normal);
        glm::vec3 blockPos = verticesT[v].Position - faceVertexOffset[face][0];
        int i = CHUNK_SIZE - 1 - (int)blockPos.z;
        int j = (int)blockPos.x;
        int k = (int)blockPos.y;
        float light = get_neighbor_light(i, j, k, face, neighbours);
        verticesT[v].LightLevel = light;
        verticesT[v+1].LightLevel = light;
        verticesT[v+2].LightLevel = light;
        verticesT[v+3].LightLevel = light;
    }

    // 仅重传 VBO 数据（几何不变，VAO/EBO 不动）
    if(!vertices.empty())
    {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());
    }
    if(!verticesT.empty())
    {
        glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, verticesT.size() * sizeof(Vertex), verticesT.data());
    }
}