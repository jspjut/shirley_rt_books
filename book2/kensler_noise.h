#ifndef KENSLERNOISEH
#define KENSLERNOISEH

namespace kensler
{
    static int const size = 256;
    static int const mask = size-1;
    
    int perm[size];
    float grads_x[size], grads_y[size], grads_z[size];
    
    void init()
    {
        // initialize gradient and permutation tables
        for (int i = 0; i < size; ++i)
        {
            int other = rand() % (i + 1);
            if (i > other)
            {
                perm[i] = perm[other];
            }
            perm[other] = i;
            grads_x[i] = cosf(2.0f * M_PI * i / size);
            grads_y[i] = sinf(2.0f * M_PI * i / size);
            // TODO: this needs to be unique...
            grads_z[i] = cosf(2.0f * M_PI * i / size);
        }
    }

    // falloff function
    float f(float t)
    {
        t = fabsf(t);
        return t >= 1.0f ? 0.0f : 1.0f - (3.0f - 2.0f * t) * t * t;
    }
    
    // surflet takes a point and returns the value for it
    float surflet(float x, float y, float grad_x, float grad_y)
    {
        return f(x) * f(y) * (grad_x * x + grad_y * y);
    }

    // single 2d noise call
    float noise2d(float x, float y)
    {
        float result = 0.0f;
        int cell_x = floorf(x);
        int cell_y = floorf(y);
        for (int grid_y = cell_y; grid_y <= cell_y + 1; ++grid_y)
        {
            for (int grid_x = cell_x; grid_x <= cell_x + 1; ++grid_x)
            {
                int hash = perm[(perm[grid_x & mask] + grid_y) & mask];
                result += surflet(x - grid_x, y - grid_y, grads_x[hash], grads_y[hash]);
            }
        }
        return result;
    }

    // turbulunce
    float turbulence2d(float x, float y, int depth=7)
    {
        float accum = 0.0f;
        float temp_x = x;
        float temp_y = y;
        float weight = 1.0;
        for (int i = 0; i < depth; ++i)
        {
            accum += weight * noise2d(temp_x, temp_y);
            weight *= 0.5;
            temp_x *= 2.0;
            temp_y *= 2.0;
        }
        return fabs(accum);
    }
}

#endif //KENSLERNOISEH
