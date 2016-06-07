#include <iostream>

// include and implement stb_image stuff
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// include my classes
#include "sphere.h"
#include "hitable_list.h"

// color and fallback to background color
vec3 color(const ray& r, hitable *world) 
{
    hit_record rec;
    if (world->hit(r, 0.0, MAXFLOAT, rec))
    {
        return 0.5*vec3(rec.normal.x()+1, rec.normal.y()+1, rec.normal.z()+1);
    }
    else
    {
        // fallback to the blue/white gradient
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5*(unit_direction.y() + 1.0);
        return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
    }
}

int main()
{
    int nx = 200;
    int ny = 100;
    // output buffer
    unsigned char *data = new unsigned char[nx*ny*3];

    // camera setup from chapter 3
    vec3 lower_left_corner(-2.0, -1.0, -1.0);
    vec3 horizontal(4.0, 0.0, 0.0);
    vec3 vertical(0.0, 2.0, 0.0);
    vec3 origin(0.0, 0.0, 0.0);

    // object list
    hitable *list[2];
    list[0] = new sphere(vec3(0,0,-1), 0.5);
    list[1] = new sphere(vec3(0,-100.5, -1), 100);
    hitable *world = new hitable_list(list, 2);

    // loop over pixels
    for (int j = ny-1; j >= 0; j--)
    {
        for (int i = 0; i < nx; i++)
        {
            float u = float(i) / float(nx);
            float v = float(j) / float(ny);
            ray r(origin, lower_left_corner + u*horizontal + v*vertical);

            // vec3 p = r.point_at_parameter(2.0);
            vec3 col = color(r, world);

            // store color in image buffer
            int ir = int(255.99*col[0]);
            int ig = int(255.99*col[1]);
            int ib = int(255.99*col[2]);
            // this fixed a bug in the chapter2 y mapping for the image
            int pix_id = (i+nx*(ny-1)-j*nx)*3;
            data[pix_id+0] = ir;
            data[pix_id+1] = ig;
            data[pix_id+2] = ib;
        }
    }

    // write buffer to file
    stbi_write_png("out.png", nx, ny, 3, data, 0);
    delete [] data;
}