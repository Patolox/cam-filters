#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "filters.h"

// Applies gray scale filter to webcam frame
void gray_scale(uint8_t* rgb_image, int width, int height)
{
    for(int i = 0; i < width * height * 3; i+=3)
    {
        int r = rgb_image[i];
        int g = rgb_image[i + 1];
        int b = rgb_image[i + 2];
        
        int g_scale = ((r + g + b) / 3.0);

        rgb_image[i] = g_scale;
        rgb_image[i + 1] = g_scale;
        rgb_image[i + 2] = g_scale;
    }
}

void dot_matrix(uint8_t* rgb_image, int width, int height)
{
    int pallete[16] = {
        0, 128, 32, 160,
        192, 64, 224, 96,
        48, 176, 16, 144,
        240, 112, 208, 80
    };
    int pallete_size = 16;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int pixel_index = (i * width + j) * 3;

            int r = rgb_image[pixel_index];
            int g = rgb_image[pixel_index + 1];
            int b = rgb_image[pixel_index + 2];

            int gray_scale = (int) round((r + g + b) / 3.0);
            
            // Creates a repeating pattern
            int threshold = pallete[(i*j) % pallete_size];

            if (gray_scale > threshold)
            {
                gray_scale = 255;
            }
            else
            {
                gray_scale = 0;
            }

            rgb_image[pixel_index] = gray_scale;
            rgb_image[pixel_index + 1] = gray_scale;
            rgb_image[pixel_index + 2] = gray_scale;
        }
    }
}

// Reflects the webcam frame
void reflect(uint8_t* rgb_image, int width, int height)
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width/2; j++)
        {
            int l_index = (i * width + j) * 3;
            int rl = rgb_image[l_index];
            int gl = rgb_image[l_index + 1];
            int bl = rgb_image[l_index + 2];

            int auxr = rl;
            int auxg = gl;
            int auxb = bl;

            int r_index = (i * width + (width - j - 1)) * 3;

            int rr = rgb_image[r_index];
            int gr = rgb_image[r_index + 1];
            int br = rgb_image[r_index + 2];

            rgb_image[l_index] = rr;
            rgb_image[l_index + 1] = gr;
            rgb_image[l_index + 2] = br;

            rgb_image[r_index] = auxr;
            rgb_image[r_index + 1] = auxg;
            rgb_image[r_index + 2] = auxb;

        }
    }
}

// Applies sepia filter to webcam frame
void sepia(uint8_t* rgb_image, int width, int height)
{

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int idx = (i * width + j) * 3;
            int sepia_r = fmin((rgb_image[idx] * .393) + (rgb_image[idx + 1] * .769) + (rgb_image[idx + 2] * .189), 255);
            int sepia_g = fmin((rgb_image[idx] * .349) + (rgb_image[idx + 1] * .686) + (rgb_image[idx + 2] * .168), 255);
            int sepia_b = fmin((rgb_image[idx] * .272) + (rgb_image[idx + 1] * .534) + (rgb_image[idx + 2] * .131), 255);

            rgb_image[idx] = sepia_r;
            rgb_image[idx + 1] = sepia_g;
            rgb_image[idx + 2] = sepia_b;
        }
    }

}

// Applies blur filter to webcam frame
void blur(uint8_t* rgb_image, int width, int height)
{
    
    const int box_blur_c = 1;

    uint8_t* temp_image = (uint8_t*)malloc(width * height * 3 * sizeof(uint8_t));
    memcpy(temp_image, rgb_image, width * height * 3);

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            float count = 0;
            int sum_r = 0;
            int sum_g = 0;
            int sum_b = 0;
            
            for (int k = i - box_blur_c; k <= i + box_blur_c; k++)
            {
                for (int m = j - box_blur_c; m <= j + box_blur_c; m++)
                {
                    int box_idx = (k * width + m) * 3;
                    if(k >= 0 && k < height && m >= 0 && m < width)
                    {
                        sum_r += temp_image[box_idx];
                        sum_g += temp_image[box_idx + 1];
                        sum_b += temp_image[box_idx + 2];
                        count++;
                    }
                }
            }
            int idx = (i * width + j) * 3;
            rgb_image[idx] = (uint8_t) round(sum_r / count);
            rgb_image[idx + 1] = (uint8_t) round(sum_g / count);
            rgb_image[idx + 2] = (uint8_t) round(sum_b / count); 

        }
    }
    free(temp_image);
}

// Applies edge filter to webcam frame
void edges(uint8_t* rgb_image, int width, int height)
{
    int count;
    int sum_bx, sum_gx, sum_rx;
    int sum_by, sum_gy, sum_ry;
    int gx_kernel[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    int gy_kernel[9] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};

    uint8_t* temp_image = (uint8_t*)malloc(width * height * 3 * sizeof(uint8_t));
    memcpy(temp_image, rgb_image, width * height * 3);

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            count = 0;
            sum_bx = sum_gx = sum_rx = 0;
            sum_by = sum_gy = sum_ry = 0;
            for (int k = i - 1; k <= i + 1; k++)
            {
                for (int m = j - 1; m <= j + 1; m++)
                {
                    if (k >= 0 && k < height && m >= 0 && m < width)
                    {
                        int box_idx = (k * width + m) * 3;

                                  // Blue
                        sum_bx += temp_image[box_idx + 2] * gx_kernel[count];
                                  // Green
                        sum_gx += temp_image[box_idx + 1] * gx_kernel[count];
                                  // Red
                        sum_rx += temp_image[box_idx] * gx_kernel[count];
                                  // Blue
                        sum_by += temp_image[box_idx + 2] * gy_kernel[count];
                                  // Green
                        sum_gy += temp_image[box_idx + 1] * gy_kernel[count];
                                  // Red
                        sum_ry += temp_image[box_idx] * gy_kernel[count];
                    }
                    count++;
                }
            }
            int idx = (i * width + j) * 3;
            rgb_image[idx + 2] = (uint8_t) fmin(round(sqrt(sum_bx * sum_bx + sum_by * sum_by)), 255.0);
            rgb_image[idx + 1] = (uint8_t) fmin(round(sqrt(sum_gx * sum_gx + sum_gy * sum_gy)), 255.0);
            rgb_image[idx] = (uint8_t) fmin(round(sqrt(sum_rx * sum_rx + sum_ry * sum_ry)), 255.0);
        }
    }
    free(temp_image);
}

// Check toggled filters in a way that enables mixes
void check_filters(int filters[], uint8_t* rgb_image, int width, int height)
{
        // bsegrd
        if (filters[0] == 1)
        {
            blur(rgb_image, width, height);
        }
        if (filters[1] == 1)
        {
            sepia(rgb_image, width, height);
        }
        if (filters[2] == 1)
        {
            edges(rgb_image, width, height);
        }
        if (filters[3] == 1)
        {
            gray_scale(rgb_image, width, height);
        }
        if (filters[4] == 1)
        {
            reflect(rgb_image, width, height);
        }
        if (filters[5] == 1)
        {
            dot_matrix(rgb_image, width, height);
        }
}