#include "tgaimage.h"
#include <cstdlib>

const TGAColor white = {
    .bgra = {255, 255, 255, 255},
};
const TGAColor red = {
    .bgra = {0, 0, 255, 255},
};

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    for (float t = 0.0; t < 1.0; t += 0.01) {
        int x = x0 * (1.0 - t) + x1 * t;
        int y = y0 * (1.0 - t) + y1 * t;
        image.set(x, y, color);
        printf("x = %d, y = %d\r\n", x, y);
    }
}

/* 遍历x坐标，插值y坐标
 * 用直线方程以x坐标为基准对线段上的点进行采样
 * (x - x0) / (x1 - x0) = t = (y - y0) / (y1 - y0)
 * 缺点：因为以x坐标采样，当(y1 - y0) > (x - x0)时采样次数 <
 * 线段上实际的像素点数量，导致线段不连续
 */
void line2(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    for (int x = x0; x <= x1; x++) {
        float t =
                (x - x0) / static_cast<float>(x1 - x0); // 避免使用整数除法，导致 t = 0
        int y = static_cast<int>(y0 * (1.0f - t) + y1 * t);
        image.set(x, y, color);
    }
}

// 遍历y坐标，插值x坐标，解决上面的缺点
void line2_transpose(int x0, int y0, int x1, int y1, TGAImage &image,
                                         TGAColor color) {
    for (int y = y0; y <= y1; y++) {
        float t =
                (y - y0) / static_cast<float>(y1 - y0); // 避免使用整数除法，导致 t = 0
        int x = static_cast<int>(x0 * (1.0f - t) + x1 * t);
        image.set(x, y, color);
    }
}

// 遍历y坐标，插值x坐标，解决上面的缺点
void line3(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);

    if (dx >= dy) {
        if (x0 < x1) {
            line2(x0, y0, x1, y1, image, color);
        } else {
            line2(x1, y1, x0, y0, image, color);
        }
    } else {
        if (y0 < y1) {
            line2_transpose(x0, y0, x1, y1, image, color);
        } else {
            line2_transpose(x1, y1, x0, y0, image, color);
        }
    }
}

int main(int argc, char **argv) {
    TGAImage image(100, 100, TGAImage::RGB);
    line2(0, 0, 100, 100, image, white); // y = x 采样次数 >= 线段上实际的像素点数量
    line2_transpose(0, 0, 50, 100, image, red); // y = 2x 采样次数 < 线段上实际的像素点数量
    line3(100, 50, 0, 0, image, white); // x0(y0)大于x1(y1)的情况

    // line2(13, 20, 80, 40, image, white);
    // line2(20, 13, 40, 80, image, red);
    // line2(80, 40, 13, 20, image, red);

    // image.flip_vertically(); // i want to have the origin at the left bottom,
    // 新代码原点默认在左下角 corner of the image
    image.write_tga_file("output.tga");
    return 0;
}
