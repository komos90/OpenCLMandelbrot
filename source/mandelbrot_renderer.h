#pragma once

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

class MandelbrotRenderer {
private:
    cl_kernel kernel_;
    cl_mem pixelGridDevice_;
    int width_;
    int height_;
    cl_command_queue queue_;
    cl_int error_;
    size_t globalWorkSize_[3];
    cl_event mainEvent_;
    std::vector<uint32_t> pixelGridHost_;
    SDL_Surface* screenSurf_;

public:
    double centreX_;
    double centreY_;
    double scale_;
    const int maxIterations_;

private:
    void genColourGradient(const std::vector<float>& positions, const std::vector<uint32_t>& colour_points, size_t len);

public:
    explicit MandelbrotRenderer(int width, int height);
    void setCentre(double centreX, double centreY);
    void setScale(double scale);
    void renderMandelbrot(SDL_Renderer* renderer);
};
