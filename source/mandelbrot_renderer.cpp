#include "mandelbrot_renderer.h"

void MandelbrotRenderer::setCentre(double centreX, double centreY) {
    centreX_ = centreX;
    centreY_ = centreY;
    clSetKernelArg(kernel_, 4, sizeof(double), &centreX_);
    clSetKernelArg(kernel_, 5, sizeof(double), &centreY_);
}

void MandelbrotRenderer::setScale(double scale) {
    scale_ = scale;
    clSetKernelArg(kernel_, 6, sizeof(double), &scale_);
}

MandelbrotRenderer::MandelbrotRenderer(int width, int height) :
    width_(width),
    height_(height),
    pixelGridHost_(width * height, 0.0),
    centreX_(0.0f),
    centreY_(0.0f),
    scale_(1.0),
    maxIterations_(1000)
{
    //Get number of platforms available
    cl_uint platformIdCount = 0;
    clGetPlatformIDs(0, nullptr, &platformIdCount);

    //Fill the platformIds vector with the platform IDs.
    std::vector<cl_platform_id> platformIds(platformIdCount);
    clGetPlatformIDs(platformIdCount, platformIds.data(), nullptr);

    //Same for devices.
    //First get the number of devices available on the 0th platform.
    cl_uint deviceIdCount = 0;
    clGetDeviceIDs(platformIds[0], CL_DEVICE_TYPE_ALL, 0, nullptr, &deviceIdCount);

    //Fill the deviceIds vector with the device IDs.
    std::vector<cl_device_id> deviceIds(deviceIdCount);
    clGetDeviceIDs(platformIds[0], CL_DEVICE_TYPE_ALL, deviceIdCount, deviceIds.data(), nullptr);

    //Fill an array of context properties with desired conotext properties.
    const cl_context_properties contextProperties[] = {
        CL_CONTEXT_PLATFORM,
        reinterpret_cast<cl_context_properties>(platformIds[0]),
        0, 0
    };
    //Create a contextfor our devices.
    cl_context context = clCreateContext(contextProperties, deviceIdCount, deviceIds.data(), nullptr, nullptr, &error_);

    //Create Buffers on GPU for our two vectors
    pixelGridDevice_ = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof(int) * (pixelGridHost_.size()),
        pixelGridHost_.data(), &error_);
    assert(error_ == CL_SUCCESS);

    //Read kernel source from a file into a std::string.
    std::ifstream inFile{ "../source/kernel/kernel.cl" };
    std::string source = "";
    std::string line = "";
    while (std::getline(inFile, line)) {
        source += line + "\n";
    }

    //Create a program object from our source code string;
    const char* source_c = source.c_str();
    cl_program program = clCreateProgramWithSource(context, 1, &source_c, nullptr, &error_);

    //Compile and link the kernel program.
    error_ = clBuildProgram(program, deviceIdCount, deviceIds.data(), nullptr, nullptr, nullptr);
    if (error_ != CL_SUCCESS) { std::cout << error_ << std::endl; }
    char build_log[1001];
    clGetProgramBuildInfo(program, deviceIds[0], CL_PROGRAM_BUILD_LOG, 1000, &build_log, nullptr);
    std::cout << build_log;
    //assert(error == CL_SUCCESS);

    //Create a kernel object from our compiled program.
    kernel_ = clCreateKernel(program, "mandelbrot", &error_);
    assert(error_ == CL_SUCCESS);

    //Create a command_queue object. Nothing special to pass here, just the device we'll be using, the context, and some flags, like whether order should be presserved.
    queue_ = clCreateCommandQueue(context, deviceIds[0], CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &error_);

    //Dimensions of array we're working with??? Should look up
    globalWorkSize_[0] = pixelGridHost_.size();
    globalWorkSize_[1] = 0;
    globalWorkSize_[2] = 0;

    //Set the arguments for our kernel.
    clSetKernelArg(kernel_, 0, sizeof(cl_mem), &pixelGridDevice_);
    clSetKernelArg(kernel_, 1, sizeof(int), &maxIterations_);
    clSetKernelArg(kernel_, 2, sizeof(int), &width_);
    clSetKernelArg(kernel_, 3, sizeof(int), &height_);
    clSetKernelArg(kernel_, 4, sizeof(double), &centreX_);
    clSetKernelArg(kernel_, 5, sizeof(double), &centreY_);
    clSetKernelArg(kernel_, 6, sizeof(double), &scale_);

    screenSurf_ = SDL_CreateRGBSurfaceFrom(pixelGridHost_.data(), width_, height_, 32, width_ * 4, 0, 0, 0, 0);
}

void MandelbrotRenderer::renderMandelbrot(SDL_Renderer* renderer) {
    error_ = clEnqueueNDRangeKernel(queue_, kernel_,
        1, nullptr,
        globalWorkSize_, nullptr,
        0, nullptr, &mainEvent_);
    assert(error_ == CL_SUCCESS);
    //When done read back result into host memory.
    clEnqueueReadBuffer(queue_, pixelGridDevice_, CL_TRUE, 0, pixelGridHost_.size() * sizeof(int), pixelGridHost_.data(), 1, &mainEvent_, nullptr);
    //assert(screenSurf != NULL);

    SDL_Texture* screenTex = SDL_CreateTextureFromSurface(renderer, screenSurf_);
    //assert(screenTex != NULL);
    SDL_RenderCopy(renderer, screenTex, NULL, NULL);
    SDL_RenderPresent(renderer);
}