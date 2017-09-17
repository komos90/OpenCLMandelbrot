#pragma OPENCL EXTENSION cl_khr_fp64 : enable

typedef struct {
    double re;
    double im;
} Complex;

double complex_abs_non_sqrt(Complex x) {
    return x.re * x.re + x.im * x.im;
}

/*Complex complex_add(Complex a, Complex b) {
    Complex result;
    result.re = a.re + b.re;
    result.im = a.im + b.im;
    return result;
}

Complex complex_mult(Complex a, Complex b) {
    Complex result;
    result.re = a.re * b.re - a.im * b.im;
    result.im = a.re * b.im + a.im * b.re;
    return result;
}*/

__kernel void mandelbrot(__global uint* pixel_grid, int max_iterations, int width, int height, double centre_x, double centre_y, double scale) {
    const int i = get_global_id(0);
    const int x = i % width;
    const int y = i / width;

    Complex c;
    c.re = (((double)x / width) * 4.0 - 2.0) * scale + centre_x;
    c.im = (((double)y / height) * 4.0 - 2.0) * scale + centre_y;

    Complex z = { .re=0.0, .im=0.0 };
    int j;
    double tmp;
    for(j = 0; j < max_iterations && complex_abs_non_sqrt(z) < 4.0; j++) {
        tmp = z.re * z.re - z.im * z.im + c.re;
        z.im = 2.0 * z.re * z.im + c.im;
        z.re = tmp;
    }
    float lVal = j / (float)max_iterations;
    pixel_grid[i] = 0xFF000000 + 0x00ffffff * (1.0f - lVal);
}