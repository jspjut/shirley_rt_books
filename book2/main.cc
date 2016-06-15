#include <iostream>
// compile with g++ main.cc -pthread

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

hitable* setup_world()
{
    // object list early chapter 10
    // hitable **list = new hitable*[2];
    // float R = cos(M_PI/4);
    // list[0] = new sphere(vec3(-R,0,-1), R, new lambertian(vec3(0,0,1)));
    // list[1] = new sphere(vec3( R,0,-1), R, new lambertian(vec3(1,0,0.0)));
    // hitable *world = new hitable_list(list, 2);

    // object list from chapter 9
    //// hitable *list[5];
    // hitable **list = new hitable*[5];
    // list[0] = new sphere(vec3(0,0,-1), 0.5, new lambertian(vec3(0.1,0.2,0.5)));
    // list[1] = new sphere(vec3(0,-100.5, -1), 100, new lambertian(vec3(0.8,0.8,0.0)));
    // list[2] = new sphere(vec3(1,0,-1), 0.5, new metal(vec3(0.8,0.6,0.2), 0.0));
    // list[3] = new sphere(vec3(-1,0,-1), 0.5, new dielectric(1.5));
    // list[4] = new sphere(vec3(-1,0,-1), -0.45, new dielectric(1.5));
    // hitable *world = new hitable_list(list, 5);

    // object list from Chapter 8.
    // hitable **list = new hitable*[4];
    // list[0] = new sphere(vec3(0,0,-1), 0.5, new lambertian(vec3(0.8,0.3,0.3)));
    // list[1] = new sphere(vec3(0,-100.5, -1), 100, new lambertian(vec3(0.8,0.8,0.0)));
    // list[2] = new sphere(vec3(1,0,-1), 0.5, new metal(vec3(0.8,0.6,0.2), 1.0));
    // list[3] = new sphere(vec3(-1,0,-1), 0.5, new metal(vec3(0.8,0.8,0.8), 0.3));
    // hitable *world = new hitable_list(list, 4);

    // Objects made with Audrey!
    hitable **list = new hitable*[12];
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

    return world;
}

#include <pthread.h>

struct prog_state
{
    hitable* world;
    unsigned char *data;
    camera *cam;
    int tid;
    int nx, ny, ns, nt;
};

void *thread_main(void *global_state)
{
    // get state passed in
    prog_state gs = *(prog_state *)global_state;
    int ns = gs.ns;
    int nx = gs.nx;
    int ny = gs.ny;
    int nt = gs.nt;
    hitable* world = gs.world;
    unsigned char* data = gs.data;
    camera cam = *gs.cam;

    srand48(gs.tid);

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
                
                col += color(r, world, 0);
            }

            // normalize color
            col /= float(ns);
            // normalize with number of threads
            col /= float(nt);
            // gamma correct
            // TODO: this needs to happen outside of the thread, 
            //       everything below will need to move
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

    pthread_exit(NULL);
}

int main()
{
    int nx = 200;
    int ny = 100;
    // int nx = 607;
    // int ny = 342;
    // int nx = 1920;
    // int ny = 1080;
    // int nx = 2000;
    // int ny = 1000;
    // sample count: 10 is very fast and noisy, 100 is reasonable (used in book)
    // 1000 is kinda slow but looks pretty good, more is probably needed for quality
    int ns = 100;
    // number of software threads (ns happens per thread)
    // TODO: need to average threads in linear space, until fixed, keep this at 1.
    int nt = 1;

    // output buffer
    unsigned char *data = new unsigned char[nx*ny*3*nt];

    // get top level hitable from world setup
    hitable *world = setup_world();

    // camera
    vec3 lookfrom(1,1,2); //3,3,2 value from book
    vec3 lookat(0,0,-1);
    float dist_to_focus = (lookfrom-lookat).length();
    float aperture = 0.25; // 2.0 value from book
    camera cam(lookfrom, lookat, vec3(0,1,0), 20, float(nx)/float(ny), aperture, dist_to_focus);

    // parallel threads
    pthread_t *threads = new pthread_t[nt];
    prog_state *pstate = new prog_state[nt];

    // loop over threads
    for (int t = 0; t < nt; t++)
    {
        // set up state
        pstate[t].tid = t;
        pstate[t].world = world;
        pstate[t].data = data + t*nx*ny*3;
        pstate[t].cam = &cam;
        pstate[t].ns = ns;
        pstate[t].nx = nx;
        pstate[t].ny = ny;
        pstate[t].nt = nt;

        // try creating thread
        int rc = pthread_create(&threads[t], NULL, thread_main, (void *)&pstate[t]);
        if (rc)
        {
            std::cerr << "ERROR; return code from pthread_create: " << rc << std::endl;
            exit(-1);
        }
    }

    // wait for threads to finish
    for (int t = 0; t < nt; t++)
    {
        pthread_join(threads[t], NULL);
    }

    // write elemental images (debugging)
    for (int t = 0; t < nt; t++)
    {
        char buf[50];
        sprintf(buf, "out%d.png", t);
        stbi_write_png(buf, nx, ny, 3, data + t*nx*ny*3, 0);
    }

    // combine images
    float scale = 1.0/nt;
    for (int j = ny-1; j >= 0; j--)
    {
        for (int i = 0; i < nx; i++)
        {
            int pix_id = (i+nx*(ny-1)-j*nx)*3;
            // data[pix_id+0] = data[pix_id+0];
            // data[pix_id+1] = data[pix_id+1];
            // data[pix_id+2] = data[pix_id+2];
            for (int t = 1; t < nt; t++)
            {
                // TODO: This summing needs to happen in linear space, not gamma space
                int offset = t*nx*ny*3;
                data[pix_id+0] += data[pix_id+offset+0];
                data[pix_id+1] += data[pix_id+offset+1];
                data[pix_id+2] += data[pix_id+offset+2];
            }
            // data[pix_id+0] = data[pix_id+0] / float(nt);
            // data[pix_id+1] = data[pix_id+1] / float(nt);
            // data[pix_id+2] = data[pix_id+2] / float(nt);
        }
    }

    // write buffer to file
    stbi_write_png("out.png", nx, ny, 3, data, 0);
    delete [] data;
}