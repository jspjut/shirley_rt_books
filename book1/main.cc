#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main()
{
    int nx = 200;
    int ny = 100;
    // output buffer
    unsigned char *data = new unsigned char[nx*ny*3];
    // set image to gradient
    for (int j = ny-1; j >= 0; j--)
    {
        for (int i = 0; i < nx; i++)
        {
            float r = float(i) / float(nx);
            float g = float(j) / float(ny);
            float b = 0.2;
            int ir = int(255.99*r);
            int ig = int(255.99*g);
            int ib = int(255.99*b);
            data[(i+j*nx)*3+0] = ir;
            data[(i+j*nx)*3+1] = ig;
            data[(i+j*nx)*3+2] = ib;
        }
    }
    // write buffer to file
    stbi_write_png("out.png", nx, ny, 3, data, 0);
    delete [] data;
}