#ifndef TEXTUREH
#define TEXTUREH

#include "ray.h"
#include "kensler_noise.h"

class texture {
    public:
        virtual vec3 value(float u, float v, const vec3& p) const = 0;
};

class constant_texture : public texture
{
    public:
        constant_texture() { }
        constant_texture(vec3 c) : color(c) { }
        virtual vec3 value(float u, float v, const vec3& p) const
        {
            return color;
        }

        vec3 color;
};

class checker_texture : public texture
{
    public:
        checker_texture() { }
        checker_texture(texture *t0, texture*t1) : even(t0), odd(t1) { }
        virtual vec3 value(float u, float v, const vec3& p) const
        {
            float sines = sin(10*p.x())*sin(10*p.y())*sin(10*p.z());
            if (sines < 0)
            {
                return odd->value(u,v,p);
            }
            else
            {
                return even->value(u,v,p);
            }
        }

        texture *even, *odd;
};

class noise_texture : public texture
{
    public:
        noise_texture() { }
        noise_texture(texture *t0, texture*t1) : zero(t0), one(t1) { }
        virtual vec3 value(float u, float v, const vec3& p) const
        {
            // change this for different noise mapping function
            float val = kensler::noise2d(p.x(), p.z());
            return val * one->value(u,v,p) + (1.0f - val) * zero->value(u,v,p);
        }

        texture *zero, *one;
};

class turb_texture : public texture
{
    public:
        turb_texture() { }
        turb_texture(texture *t0, texture*t1) : zero(t0), one(t1) { }
        virtual vec3 value(float u, float v, const vec3& p) const
        {
            // change this for different noise mapping function
            float val = kensler::turbulence2d(p.x(), p.z());
            return val * one->value(u,v,p) + (1.0f - val) * zero->value(u,v,p);
        }

        texture *zero, *one;
};

class marble_texture : public texture
{
    public:
        marble_texture() { }
        marble_texture(texture *t0, texture*t1) : zero(t0), one(t1) { }
        virtual vec3 value(float u, float v, const vec3& p) const
        {
            // change this for different noise mapping function
            float tval = kensler::turbulence2d(p.x(), p.z());
            float val = 0.5 * (1.0 + sin(8*p.x()+8*p.y()+2*p.z() + 8*tval));
            // lerp
            return val * one->value(u,v,p) + (1.0f - val) * zero->value(u,v,p);
        }

        texture *zero, *one;
};

#endif //TEXTUREH