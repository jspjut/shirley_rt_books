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

// abstract class
class material
{
    public:
        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const = 0;
};

class lambertian : public material {
    public:
        lambertian(const vec3& a) : albedo(a) {}
        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const
        {
            vec3 target = rec.p + rec.normal + random_in_unit_sphere();
            scattered = ray(rec.p, target - rec.p);
            attenuation = albedo;
            return true;
        }

        vec3 albedo;
};

vec3 reflect(const vec3& v, const vec3& n)
{
    return v - 2*dot(v,n)*n;
}

class metal : public material {
    public:
        metal(const vec3& a, float f) : albedo(a) { if (f<1) fuzz = f; else fuzz = 1; }
        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const
        {
            vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
            scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere());
            attenuation = albedo;
            return (dot(scattered.direction(), rec.normal) > 0);
        }

        vec3 albedo;
        float fuzz;
};

// returns true if refraction happens. Also returns the refraction direction.
bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted)
{
    vec3 uv = unit_vector(v);
    float dt = dot(uv, n);
    float discriminant = 1.0 - ni_over_nt*ni_over_nt*(1-dt*dt);
    if (discriminant > 0)
    {
        refracted = ni_over_nt*(uv - n*dt) - n*sqrt(discriminant);
        return true;
    }
    else
    {
        return false;
    }
}

class dielectric : public material
{
    public:
        dielectric(float ri) : ref_idx(ri) {}
        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const
        {
            vec3 outward_normal;
            vec3 reflected = reflect(r_in.direction(), rec.normal);
            float ni_over_nt;
            attenuation = vec3(1.0, 1.0, 1.0);
            vec3 refracted;
            // check which side of normal the ray is
            if (dot(r_in.direction(), rec.normal) > 0)
            {
                outward_normal = -rec.normal;
                ni_over_nt = ref_idx;
            }
            else
            {
                outward_normal = rec.normal;
                ni_over_nt = 1.0 / ref_idx;                
            }
            // check if refraction happens
            if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted))
            {
                scattered = ray(rec.p, refracted);
            }
            else
            {
                scattered = ray(rec.p, reflected);
                // return false; // false to avoid reflections for now...
            }
            return true;
        }

        float ref_idx;
};

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
    // int nx = 2000;
    // int ny = 1000;
    // sample count: 10 is very fast and noisy, 100 is reasonable (used in book)
    // 1000 is kinda slow but looks pretty good, more is probably needed for quality
    int ns = 100;

    // output buffer
    unsigned char *data = new unsigned char[nx*ny*3];

    // object list
    hitable *list[4];
    list[0] = new sphere(vec3(0,0,-1), 0.5, new lambertian(vec3(0.1,0.2,0.5)));
    list[1] = new sphere(vec3(0,-100.5, -1), 100, new lambertian(vec3(0.8,0.8,0.0)));
    list[2] = new sphere(vec3(1,0,-1), 0.5, new metal(vec3(0.8,0.6,0.2), 0.0));
    list[3] = new sphere(vec3(-1,0,-1), 0.5, new dielectric(1.5));
    hitable *world = new hitable_list(list, 4);

    // object list from Chapter 8.
    // hitable *list[4];
    // list[0] = new sphere(vec3(0,0,-1), 0.5, new lambertian(vec3(0.8,0.3,0.3)));
    // list[1] = new sphere(vec3(0,-100.5, -1), 100, new lambertian(vec3(0.8,0.8,0.0)));
    // list[2] = new sphere(vec3(1,0,-1), 0.5, new metal(vec3(0.8,0.6,0.2), 1.0));
    // list[3] = new sphere(vec3(-1,0,-1), 0.5, new metal(vec3(0.8,0.8,0.8), 0.3));
    // hitable *world = new hitable_list(list, 4);

    // Objects made with Audrey!
    // hitable *list[6];
    // list[0] = new sphere(vec3(0,-100.5, -1), 100, new lambertian(vec3(0.545,0.27,0.075)));//139, 69, 19
    // // upper row blue, black, red
    // list[1] = new sphere(vec3(-0.55,0.0,-1), 0.25, new metal(vec3(0.0,0.4,0.8), 0.9));
    // list[2] = new sphere(vec3(0,0.0,-1), 0.25, new metal(vec3(0.116,0.1,0.1), 0.9));
    // list[3] = new sphere(vec3(0.55,0.0,-1), 0.25, new metal(vec3(0.875,0.208,0.29), 0.9));
    // // lower row yellow, green
    // list[4] = new sphere(vec3(-0.275,-0.25,-1), 0.25, new metal(vec3(0.953,0.714,0.302), 0.7));
    // list[5] = new sphere(vec3(0.275,-0.25,-1), 0.25, new metal(vec3(0.114,0.613,0.333), 0.7));
    // hitable *world = new hitable_list(list, 6);

    // camera
    camera cam;

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