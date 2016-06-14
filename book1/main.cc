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
#include "material.h"

// color and fallback to background color
vec3 color(const ray& r, hitable *world, int depth) 
{
    hit_record rec;
    // ignore small t values
    if (world->hit(r, 0.001, MAXFLOAT, rec))
    {
        // material shading
        ray scattered;
        vec3 attenuation;
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        {
            if (scattered.direction().length() == 0)
            {
                //special case for emitter
                return attenuation;
            }
            return attenuation*color(scattered, world, depth+1);
        }
        else
        {
            return vec3(0,0,0);
        }

        // diffuse shading
        // vec3 target = rec.p + rec.normal + random_in_unit_sphere();
        // return 0.5*color( ray(rec.p, target - rec.p), world);

        // normal shading
        // return 0.5*vec3(rec.normal.x()+1, rec.normal.y()+1, rec.normal.z()+1);
    }
    else
    {
        // fallback to the blue/white gradient (sky)
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5*(unit_direction.y() + 1.0);
        return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
    }
}

int main()
{
    int nx = 200;
    int ny = 100;
    // int nx = 2000;
    // int ny = 1000;
    // sample count: 10 is very fast and noisy, 100 is reasonable (used in book)
    // 1000 is kinda slow but looks pretty good, more is probably needed for quality
    int ns = 100;

    // output buffer
    unsigned char *data = new unsigned char[nx*ny*3];

    // object list early chapter 10
    // hitable *list[2];
    // float R = cos(M_PI/4);
    // list[0] = new sphere(vec3(-R,0,-1), R, new lambertian(vec3(0,0,1)));
    // list[1] = new sphere(vec3( R,0,-1), R, new lambertian(vec3(1,0,0.0)));
    // hitable *world = new hitable_list(list, 2);

    // object list from chapter 9
    // hitable *list[5];
    // list[0] = new sphere(vec3(0,0,-1), 0.5, new lambertian(vec3(0.1,0.2,0.5)));
    // list[1] = new sphere(vec3(0,-100.5, -1), 100, new lambertian(vec3(0.8,0.8,0.0)));
    // list[2] = new sphere(vec3(1,0,-1), 0.5, new metal(vec3(0.8,0.6,0.2), 0.0));
    // list[3] = new sphere(vec3(-1,0,-1), 0.5, new dielectric(1.5));
    // list[4] = new sphere(vec3(-1,0,-1), -0.45, new dielectric(1.5));
    // hitable *world = new hitable_list(list, 5);

    // object list from Chapter 8.
    // hitable *list[4];
    // list[0] = new sphere(vec3(0,0,-1), 0.5, new lambertian(vec3(0.8,0.3,0.3)));
    // list[1] = new sphere(vec3(0,-100.5, -1), 100, new lambertian(vec3(0.8,0.8,0.0)));
    // list[2] = new sphere(vec3(1,0,-1), 0.5, new metal(vec3(0.8,0.6,0.2), 1.0));
    // list[3] = new sphere(vec3(-1,0,-1), 0.5, new metal(vec3(0.8,0.8,0.8), 0.3));
    // hitable *world = new hitable_list(list, 4);

    // Objects made with Audrey!
    hitable *list[12];
    list[0] = new sphere(vec3(0,-100.5, -1), 100, new lambertian(vec3(0.545,0.27,0.075)));//139, 69, 19
    // upper row blue, black, red
    float r1 = 0.1, r2 = 0.25;
    list[1] = new sphere(vec3(-0.55,0.0,-1), r1, new metal(vec3(0.0,0.4,0.8), 0.9));
    list[7] = new sphere(vec3(-0.55,0.0,-1), r2, new dielectric(1.5));
    list[2] = new sphere(vec3(0,0.0,-1), r1, new metal(vec3(0.116,0.1,0.1), 0.9));
    list[8] = new sphere(vec3(0,0.0,-1), r2, new dielectric(1.5));
    list[3] = new sphere(vec3(0.55,0.0,-1), r1, new metal(vec3(0.875,0.208,0.29), 0.9));
    list[9] = new sphere(vec3(0.55,0.0,-1), r2, new dielectric(1.5));
    // lower row yellow, green
    list[4] = new sphere(vec3(-0.275,-0.25,-1), r1, new metal(vec3(0.953,0.714,0.302), 0.7));
    list[10] = new sphere(vec3(-0.275,-0.25,-1), r2, new dielectric(1.5));
    list[5] = new sphere(vec3(0.275,-0.25,-1), r1, new metal(vec3(0.114,0.613,0.333), 0.7));
    list[11] = new sphere(vec3(0.275,-0.25,-1), r2, new dielectric(1.5));
    // metal
    // // upper row blue, black, red
    // list[1] = new sphere(vec3(-0.55,0.0,-1), 0.25, new metal(vec3(0.0,0.4,0.8), 0.9));
    // list[2] = new sphere(vec3(0,0.0,-1), 0.25, new metal(vec3(0.116,0.1,0.1), 0.9));
    // list[3] = new sphere(vec3(0.55,0.0,-1), 0.25, new metal(vec3(0.875,0.208,0.29), 0.9));
    // // lower row yellow, green
    // list[4] = new sphere(vec3(-0.275,-0.25,-1), 0.25, new metal(vec3(0.953,0.714,0.302), 0.7));
    // list[5] = new sphere(vec3(0.275,-0.25,-1), 0.25, new metal(vec3(0.114,0.613,0.333), 0.7));
    // sky
    list[6] = new sphere(vec3(0,-100.5, -1), 1000, new light_source(vec3(0.6,0.6,0.7)));//139, 69, 19
    hitable *world = new hitable_list(list, 12);

    // camera
    vec3 lookfrom(1,1,2); //3,3,2 value from book
    vec3 lookat(0,0,-1);
    float dist_to_focus = (lookfrom-lookat).length();
    float aperture = 0.25; // 2.0 value from book
    camera cam(lookfrom, lookat, vec3(0,1,0), 20, float(nx)/float(ny), aperture, dist_to_focus);

    // loop over pixels
    // first hit for debugging
    // int j = 78;
    // int i = 22;
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
                
                // not sure why this line was in the book
                // vec3 p = r.point_at_parameter(2.0);
                col += color(r, world, 0);
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