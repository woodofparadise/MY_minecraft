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

        double V1 = fastDot(aa, distAA);
        double V2 = fastDot(ab, distAB);
        double V3 = fastDot(ba, distBA);
        double V4 = fastDot(bb, distBB);

        double u = fade(xf), v = fade(yf);
        double V12 = V1 + u*(V2-V1);
        double V34 = V3 + u*(V4-V3);
        // cout << V12 << " " << V34 << " " << u << " " << v << endl;
        double result = V12 + v*(V34-V12);

        return result;
    }

    double fastDot(int index, glm::vec2 dist)
    {
        switch(index & 0x8)
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
    
};

#endif