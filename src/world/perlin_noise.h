#ifndef PERLIN_NOISE_H
#define PERLIN_NOISE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include<vector>
#include<stdlib.h>
#include<math.h>

using namespace std;
class PerlinNoice
{
private:
    int seed;
    // const glm::vec2 gradients[] = {
    //     {1, 1}, {-1, 1}, {1, -1}, {-1, -1},
    //     {1, 0}, {-1, 0}, {0, 1}, {0, -1}
    // };
    int perm[512];
public:
    PerlinNoice()
    {
        this->seed = 999;
        for(int i = 0; i < 256; i++) 
        {
            perm[i] = i;
        }
    }

    void set_seed(int seed)
    {
        // 根据设定的随机数种子进行打乱
        this->seed = seed;
        srand(this->seed);
        for(int i = 0; i < 256; i++) 
        {
            int j = rand() % 256;
            swap(perm[i], perm[j]);
        }
        
        // 扩展到512长度
        for(int i = 0; i < 256; i++)
        {
            perm[256 + i] = perm[i];
        }
    }

    double get_2D_perlin_noice(double x, double y)
    {
        int xi = (int)floor(x) & 255;
        int yi = (int)floor(y) & 255;

        double xf = x - floor(x);
        double yf = y - floor(y);

        // 四个角点的梯度向量索引哈希值
        int aa = perm[perm[xi] + yi];           // 左下
        int ab = perm[perm[xi] + yi + 1];       // 左上
        int ba = perm[perm[xi + 1] + yi];       // 右下
        int bb = perm[perm[xi + 1] + yi + 1];   // 右上

        // 到四个角点的向量
        glm::vec2 distAA = glm::vec2(xf, yf);
        glm::vec2 distAB = glm::vec2(xf, yf - 1);
        glm::vec2 distBA = glm::vec2(xf - 1, yf);
        glm::vec2 distBB = glm::vec2(xf - 1, yf - 1);

        double V1 = fast_dot(aa, distAA);
        double V2 = fast_dot(ab, distAB);
        double V3 = fast_dot(ba, distBA);
        double V4 = fast_dot(bb, distBB);

        double u = fade(xf), v = fade(yf);
        double V12 = V1 + v*(V2-V1);
        double V34 = V3 + v*(V4-V3);
        // cout << V12 << " " << V34 << " " << u << " " << v << endl;
        double result = V12 + u*(V34-V12);

        return result;
    }

    double fast_dot(int index, glm::vec2 dist)
    {
        // cout << index << " " << (index&0x7) << endl;
        switch(index & 0x7)
        {
            case 0x0: return dist.x + dist.y;
            case 0x1: return -dist.x + dist.y;
            case 0x2: return  dist.x - dist.y;
            case 0x3: return -dist.x - dist.y;
            case 0x4: return  dist.x;
            case 0x5: return -dist.x;
            case 0x6: return  dist.y;
            case 0x7: return -dist.y;
            default: return 0; // never happens
        }
        return 0;
    }

    double fade(double x)
    {
        return 6*pow(x, 5) - 15*pow(x, 4) + 10*pow(x, 3);
    }
    
    double get_fbm_noise(double x, double y, int octaves, double persistence, double lacunarity) 
    {
        double total = 0.0;
        double amplitude = 1.0;
        double frequency = 1.0;
        double maxValue = 0.0;

        for(int i = 0; i < octaves; i++) 
        {
            total += get_2D_perlin_noice(x * frequency, y * frequency) * amplitude;
            maxValue += amplitude;
            amplitude *= persistence;  // 每层振幅衰减
            frequency *= lacunarity;   // 每层频率增加
        }
        return total / maxValue;  // 归一化
    }

    double fast_dot_3D(int index, glm::vec3 dist)
    {
        switch(index & 0xF)
        {
            case 0x0: return dist.x + dist.y;
            case 0x1: return -dist.x + dist.y;
            case 0x2: return  dist.x - dist.y;
            case 0x3: return -dist.x - dist.y;
            case 0x4: return  dist.x + dist.z;
            case 0x5: return -dist.x + dist.z;
            case 0x6: return  dist.x - dist.z;
            case 0x7: return -dist.x - dist.z;
            case 0x8: return  dist.y + dist.z;
            case 0x9: return -dist.y + dist.z;
            case 0xA: return  dist.y - dist.z;
            case 0xB: return -dist.y - dist.z;
        }
        return 0;
    }

    double lerp(double a, double b, double t)
    {
        return a + t * (b - a);
    }

    double get_3D_perlin_noice(double x, double y, double z)
    {
        int xi = (int)floor(x) & 255;
        int yi = (int)floor(y) & 255;
        int zi = (int)floor(z) & 255;

        double xf = x - floor(x);
        double yf = y - floor(y);
        double zf = z - floor(z);

        int aaa = perm[perm[perm[xi] + yi] + zi];         // (0,0,0)
        int aab = perm[perm[perm[xi] + yi] + zi + 1];     // (0,0,1)
        int aba = perm[perm[perm[xi] + yi + 1] + zi];     // (0,1,0)
        int abb = perm[perm[perm[xi] + yi + 1] + zi + 1]; // (0,1,1)
        int baa = perm[perm[perm[xi + 1] + yi] + zi];     // (1,0,0)
        int bab = perm[perm[perm[xi + 1] + yi] + zi + 1]; // (1,0,1)
        int bba = perm[perm[perm[xi + 1] + yi + 1] + zi]; // (1,1,0)
        int bbb = perm[perm[perm[xi + 1] + yi + 1] + zi + 1]; // (1,1,1)


        glm::vec3 v000 = glm::vec3(xf,     yf,     zf);
        glm::vec3 v001 = glm::vec3(xf,     yf,     zf - 1);
        glm::vec3 v010 = glm::vec3(xf,     yf - 1, zf);
        glm::vec3 v011 = glm::vec3(xf,     yf - 1, zf - 1);
        glm::vec3 v100 = glm::vec3(xf - 1, yf,     zf);
        glm::vec3 v101 = glm::vec3(xf - 1, yf,     zf - 1);
        glm::vec3 v110 = glm::vec3(xf - 1, yf - 1, zf);
        glm::vec3 v111 = glm::vec3(xf - 1, yf - 1, zf - 1);

        double n000 = fast_dot_3D(aaa, v000);
        double n001 = fast_dot_3D(aab, v001);
        double n010 = fast_dot_3D(aba, v010);
        double n011 = fast_dot_3D(abb, v011);
        double n100 = fast_dot_3D(baa, v100);
        double n101 = fast_dot_3D(bab, v101);
        double n110 = fast_dot_3D(bba, v110);
        double n111 = fast_dot_3D(bbb, v111);

        double u = fade(xf), v = fade(yf), w = fade(zf);

        // 先沿x轴插值（4次）
        double x00 = lerp(n000, n100, u);
        double x01 = lerp(n001, n101, u);
        double x10 = lerp(n010, n110, u);
        double x11 = lerp(n011, n111, u);

        // 再沿y轴插值（2次）
        double y0 = lerp(x00, x10, v);
        double y1 = lerp(x01, x11, v);

        // 最后沿z轴插值（1次）
        double result = lerp(y0, y1, w);

        return result;
    }

};

#endif