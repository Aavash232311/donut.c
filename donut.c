#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>

#define M_2PI (3.14159265358979323846 * 2.0)

// once in a while I love to do C
typedef struct
{
    float x;
    float y;
} Vector2;

typedef struct
{
    float x;
    float y;
    float z;
} Vector3;

Vector2 drawCircle(int h, int k, int r, float theta)
{
    float x = h + r * cosf(theta);
    float y = k + r * sinf(theta);
    Vector2 resultant;
    resultant.x = x;
    resultant.y = y;
    return resultant;
}

void rotate_z(float theta, float out[3][3])
{
    float c = cosf(theta);
    float s = sinf(theta);

    // matrix rotation around z formula

    out[0][0] = c;
    out[0][1] = -s;
    out[0][2] = 0;
    out[1][0] = s;
    out[1][1] = c;
    out[1][2] = 0;
    out[2][0] = 0;
    out[2][1] = 0;
    out[2][2] = 1;
}

// just pure formula look for wiki more info
void rotate_x(float theta, float out[3][3])
{
    float c = cosf(theta);
    float s = sinf(theta);

    out[0][0] = 1;
    out[0][1] = 0;
    out[0][2] = 0;
    out[1][0] = 0;
    out[1][1] = c;
    out[1][2] = -s;
    out[2][0] = 0;
    out[2][1] = s;
    out[2][2] = c;
}

void mat3_mul_vec(float M[3][3], Vector3 v, Vector3 *out)
{
    /* 
     mat M = [a, b, c]
             [d, e, f]
             [g, h, i]
     mat V = [x]
             [y]
             [z]

    resultant = [ax + by + cz]
                [dx + ey + fz]
                [gx + hy + iz]
    */
    out->x = M[0][0] * v.x + M[0][1] * v.y + M[0][2] * v.z;
    out->y = M[1][0] * v.x + M[1][1] * v.y + M[1][2] * v.z;
    out->z = M[2][0] * v.x + M[2][1] * v.y + M[2][2] * v.z;
}

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    { // initilizes library
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    // creates a window, the other ags 500, 100 is position of window on screen.
    // 400x400 is height and width of window in pixel
    int width_frame = 800;
    int height_frame = 400;

    SDL_Window *win = SDL_CreateWindow("donut.c", 500, 200, width_frame, height_frame, SDL_WINDOW_SHOWN);
    if (!win)
    {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    // SDL_RENDERER_ACCELERATED: use GPU for fast drawing, SDL_RENDERER_PRESENTVSYNC Sync rendering to the monitor refresh rate
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ren)
    {
        SDL_DestroyWindow(win);
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Event event;
    int quit = 0;

    Uint32 frame_start;
    Uint32 frame_time;
    float current_fps = 0.0f;

    int dim = 1; // point dimension

    int r = 30;

    double step_size = 0.05;
    int major_axis = 60;

    float theta = 0.0f;
    float tau = 0.0;

    int c = 0;

    while (!quit)
    {
        frame_start = SDL_GetTicks();

        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);

        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);

        Vector3 points[15876]; // pre-allocating memory I don't want to deal with dynamic allocation

        int i = 0;
        while (theta < M_2PI)
        { // completed turn in std measure of angle

            float phi = 0;

            while (phi < M_2PI) // goes around small circle, def of tours
            {
                Vector3 tours;
                points[i].x = (major_axis + r * cosf(theta)) * cosf(phi);
                points[i].y = r * sinf(theta);
                points[i].z = -(major_axis + r * cosf(theta)) * sinf(phi);

                phi += step_size;
                i++;
            }
            theta += step_size;
            c++;
        }

        for (int i = 0; i < 15876; i++)
        {
            Vector3 point = points[i];
            Vector3 resultant;

            /* I am too old and lazy for putting text there,
            although idk how but can be done if we iterate around some
            int and print them as ASCII characters */

            Vector3 resX;
            float transofrm_x[3][3], transofrm_z[3][3];

            rotate_x(tau, transofrm_x);
            mat3_mul_vec(transofrm_x, point, &resX);
            
            rotate_z(tau, transofrm_z);
            mat3_mul_vec(transofrm_z, resX, &resultant);



            int init_x = (width_frame / 2) + resultant.x;
            int init_y = (height_frame / 2) + resultant.y;

            Vector2 pos = drawCircle(init_x, init_y, r, tau);

            SDL_Rect rect = {pos.x, pos.y, dim, dim};
            SDL_RenderFillRect(ren, &rect);
        }

        tau += 0.01;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                quit = 1;
        }

        SDL_RenderPresent(ren);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}