#include <iostream>

// include and implement stb_image stuff
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// include ray and vec3 classes
#include "ray.h"

bool hit_sphere(const vec3& center, float radius, const ray& r)
{
    vec3 oc = r.origin() - center;
    float a = dot(r.direction(), r.direction());
    float b = 2.0 * dot(oc, r.direction());
    float c = dot(oc, oc) - radius*radius;
    float discriminant = b*b - 4*a*c;
    return (discriminant > 0);
}

// test whether a sphere is hit
vec3 color(const ray& r) 
{
    if (hit_sphere(vec3(0,0,-1), 0.5, r))
        return vec3(1, 0, 0);
    // fallback to the blue/white gradient
    vec3 unit_direction = unit_vector(r.direction());
    float t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
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

    // set image to gradient
    for (int j = ny-1; j >= 0; j--)
    {
        for (int i = 0; i < nx; i++)
        {
            float u = float(i) / float(nx);
            float v = float(j) / float(ny);
            ray r(origin, lower_left_corner + u*horizontal + v*vertical);
            vec3 col = color(r);
            // vec3 col(float(i)/float(nx), float(j)/float(ny), 0.2f);

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