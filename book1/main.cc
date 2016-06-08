#include <iostream>

// include and implement stb_image stuff
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// include my classes
#include "sphere.h"
#include "hitable_list.h"
#include "camera.h"

// choose a random vector in the unit sphere
vec3 random_in_unit_sphere()
{
    vec3 p;
    do {
        p = 2.0*vec3(drand48(),drand48(),drand48()) - vec3(1,1,1);
    } while (p.squared_length() >= 1.0);
    return p;
}

// color and fallback to background color
vec3 color(const ray& r, hitable *world) 
{
    hit_record rec;
    // ignore small t values
    if (world->hit(r, 0.001, MAXFLOAT, rec))
    {
        // diffuse shading
        vec3 target = rec.p + rec.normal + random_in_unit_sphere();
        return 0.5*color( ray(rec.p, target - rec.p), world);
        // normal shading
        // return 0.5*vec3(rec.normal.x()+1, rec.normal.y()+1, rec.normal.z()+1);
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
    // sample count: 10 is very fast and noisy, 100 is reasonable (used in book)
    // 1000 is kinda slow but looks pretty good, more is probably needed for quality
    int ns = 100;

    // output buffer
    unsigned char *data = new unsigned char[nx*ny*3];

    // object list
    hitable *list[2];
    list[0] = new sphere(vec3(0,0,-1), 0.5);
    list[1] = new sphere(vec3(0,-100.5, -1), 100);
    hitable *world = new hitable_list(list, 2);

    // camera
    camera cam;

    // loop over pixels
    for (int j = ny-1; j >= 0; j--)
    {
        for (int i = 0; i < nx; i++)
        {
            vec3 col(0,0,0);

            for (int s=0; s < ns; s++)
            {
                float u = float(i + drand48()) / float(nx);
                float v = float(j + drand48()) / float(ny);

                ray r = cam.get_ray(u, v);
                vec3 p = r.point_at_parameter(2.0);
                col += color(r, world);
            }
            // normalize color
            col /= float(ns);
            // gamma correct
            col = vec3( sqrt(col[0]), sqrt(col[1]), sqrt(col[2]) );

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