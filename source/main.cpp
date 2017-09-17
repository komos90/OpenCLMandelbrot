#ifdef __APPLE__
    #include "OpenCL/opencl.h"
#else
    #include "CL/cl.h"
#endif

#include <SDL.h>

#include <cassert>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <complex>

#include "consts.h"
#include "mandelbrot_renderer.h"

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO);
    SDL_Renderer* renderer;
    SDL_Window* window;
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, NULL, &window, &renderer);
    SDL_SetWindowTitle(window, "OpenCL Mandelbrot Explorer");

    MandelbrotRenderer mr(600, 600);
    mr.renderMandelbrot(renderer);
    
    bool running = true;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    float cenAdjX = e.motion.x / (float)SCREEN_WIDTH;
                    float cenAdjY = e.motion.y / (float)SCREEN_HEIGHT;

                    mr.setCentre(mr.centreX_ - mr.scale_ * 2 + cenAdjX * 4 * mr.scale_, mr.centreY_ - mr.scale_ * 2 + cenAdjY * 4 * mr.scale_);
                    
                    mr.setScale(0.8 * mr.scale_);
                    mr.renderMandelbrot(renderer);
                }
            }
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_SPACE:
                case SDLK_RETURN: {
                    mr.setScale(mr.scale_ *0.8);
                    mr.renderMandelbrot(renderer);
                    break;
                }
                case SDLK_LEFT: {
                    mr.setCentre(mr.centreX_ - mr.scale_ / 4.0, mr.centreY_);
                    mr.renderMandelbrot(renderer);
                    break;
                }
                case SDLK_RIGHT: {
                    mr.setCentre(mr.centreX_ + mr.scale_ / 4.0, mr.centreY_);
                    mr.renderMandelbrot(renderer);
                    break;
                }
                case SDLK_UP: {
                    mr.setCentre(mr.centreX_, mr.centreY_ - mr.scale_ / 4.0);
                    mr.renderMandelbrot(renderer);
                    break;
                }
                case SDLK_DOWN: {
                    mr.setCentre(mr.centreX_, mr.centreY_ + mr.scale_ / 4.0);
                    mr.renderMandelbrot(renderer);
                    break;
                }
                }
            }
        }
    }

    return 0;
}