#include <stdint.h>

void gray_scale(uint8_t* rgb_image, int width, int height);
void dot_matrix(uint8_t* rgb_image, int width, int height);
void reflect(uint8_t* rgb_image, int width, int height);
void sepia(uint8_t* rgb_image, int width, int height);
void blur(uint8_t* rgb_image, int width, int height);
void edges(uint8_t* rgb_image, int width, int height);
void check_filters(int filters[], uint8_t* rgb_image, int width, int height);