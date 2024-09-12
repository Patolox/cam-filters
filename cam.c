#include <SDL2/SDL.h>

#include "filters.h"
#include "camv4l.h"

#define S_WIDTH 640
#define S_HEIGHT 360


int main(void) {
    int fd = open("/dev/video0", O_RDWR);  // Open the video device with read/write access

    if (fd == -1) {
        perror("Opening video device");
        exit(1);
    }

    query_cap(fd);
    set_fmt(fd, S_WIDTH, S_HEIGHT);

    // Request buffer and queue them all
    int buffer_count = req_buffer(fd);
    struct v4l2_buffer buf;
    void* buffer_start[buffer_count];
    queue_buffers(fd, &buf, buffer_count, buffer_start);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Failed to initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Webcam", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, S_WIDTH*2, S_HEIGHT*2, SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
        return 2;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
        return 3;
    }

    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, S_WIDTH, S_HEIGHT);
    if (!texture) {
        fprintf(stderr, "Failed to create texture: %s\n", SDL_GetError());
        return 4;
    }

    // Start capture
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) == -1) {
        perror("Starting Capture");
        exit(1);
    }

    SDL_Event event;
    int running = 1;
    int filters[6] = {0};
    
    while (running) {

         while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                printf("Exit event received, closing...\n");
                running = 0;
            }
            else if(event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym) {
                    case SDLK_b:
                        filters[0] = !filters[0];
                        printf("Blur toggled: %d\n", filters[0]);
                        break;
                    case SDLK_s:
                        filters[1] = !filters[1];
                        printf("Sepia toggled: %d\n", filters[1]);
                        break;
                    case SDLK_e:
                        filters[2] = !filters[2];
                        printf("Edges toggled: %d\n", filters[2]);
                        break;
                    case SDLK_g:
                        filters[3] = !filters[3];
                        printf("Gray scale toggled: %d\n", filters[3]);
                        break;
                    case SDLK_r:
                        filters[4] = !filters[4];
                        printf("Reflection toggled: %d\n", filters[4]);
                        break;
                    case SDLK_d:
                        filters[5] = !filters[5];
                        printf("Dot matrix toggled: %d\n", filters[5]);
                        break;
                    default:
                        break;
                }
            }
        }

        // This line only sets all bytes of buf to 0
        memset(&buf, 0, sizeof(buf));

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (ioctl(fd, VIDIOC_DQBUF, &buf) == -1) {
            perror("Retrieving Frame");
            exit(1);
        }

        uint8_t* rgb_image = yuv_to_rgb(buffer_start[buf.index], S_WIDTH, S_HEIGHT);

        check_filters(filters, rgb_image, S_WIDTH, S_HEIGHT);

        // Update the SDL texture with the YUYV frame
        SDL_UpdateTexture(texture, NULL, rgb_image, S_WIDTH * 3);

        // Requeue the buffer
        if (ioctl(fd, VIDIOC_QBUF, &buf) == -1) {
            perror("Requeueing Buffer");
            exit(1);
        }

        SDL_Rect src_rect = { 0, 0, S_WIDTH, S_HEIGHT };
        SDL_Rect dst_rect = { 0, 0, S_WIDTH * 2, S_HEIGHT * 2 };

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, &src_rect, &dst_rect);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    if (ioctl(fd, VIDIOC_STREAMOFF, &type) == -1) {
        perror("Stopping Capture");
        exit(1);
    }

    for (int i = 0; i < buffer_count; i++) {
        munmap(buffer_start[i], buf.length);
    }

    close(fd);
    return 0;
}
