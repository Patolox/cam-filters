#include "camv4l.h"

// Query webcam capabilities...
int query_cap(int fd) {
    struct v4l2_capability cap;
    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1) {
        perror("Querying Capabilities");
        exit(1);
    }
    return 0;
}

// Set format for the webcam
int set_fmt(int fd, int width, int height) {
    struct v4l2_format fmt;

    // This line only sets all bytes on fmt to 0
    memset(&fmt, 0, sizeof(fmt));

    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = width;
    fmt.fmt.pix.height = height;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV; // YUYV format
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

    if (ioctl(fd, VIDIOC_S_FMT, &fmt) == -1) {
        perror("Setting pixel format");
        exit(1);
    }
    return 0;
}

// This function requests buffers to the device.
// It says to the webcam that we will use 4 buffers
// To store frames, the application will iterate over
// these 4 buffers to save and display the image seamlessly
// (They are image buffers).
int req_buffer(int fd) {
    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof(req));
    req.count = 4; // Number of buffers
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (ioctl(fd, VIDIOC_REQBUFS, &req) == -1) {
        perror("Requesting Buffer");
        exit(1);
    }
    return req.count;
}

// This function queues the buffers we requested earlier,
// we need to do this so the program knows that there are buffers
// to be used
int queue_buffers(int fd, struct v4l2_buffer* buf, int buffer_count, void** buffer_start) {
    for (int i = 0; i < buffer_count; i++) {

        // This line only sets all bytes on buf to 0
        memset(buf, 0, sizeof(*buf));

        buf->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf->memory = V4L2_MEMORY_MMAP;
        buf->index = i;

        if (ioctl(fd, VIDIOC_QUERYBUF, buf) == -1) {
            perror("Querying Buffer");
            exit(1);
        }

        buffer_start[i] = mmap(NULL, buf->length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf->m.offset);
        if (buffer_start[i] == MAP_FAILED) {
            perror("Memory Mapping");
            exit(1);
        }

        // Queue the buffer after mapping
        if (ioctl(fd, VIDIOC_QBUF, buf) == -1) {
            perror("Queueing Buffer");
            exit(1);
        }
    }
    return 0;
}

// Transforms yuv frame to rgb frame
uint8_t* yuv_to_rgb(void* buffer_start, int width, int height) {
    // Allocate space for the RGB image
    int rgb_size = width * height * 3;  // 3 bytes per pixel (RGB)
    uint8_t* rgb_image = malloc(rgb_size);

    if (rgb_image == NULL) {
        perror("Failed to allocate memory for RGB image");
        return NULL;
    }

    uint8_t* yuyv_data = (uint8_t*) buffer_start;
    
    // Loop through the YUYV buffer
    for (int i = 0, j = 0; i < width * height * 2; i += 4, j += 6) {
        int y0 = yuyv_data[i] - 16;
        int u  = yuyv_data[i + 1] - 128;
        int y1 = yuyv_data[i + 2] - 16;
        int v  = yuyv_data[i + 3] - 128;

        // Convert the first pixel (Y0, U, V)
        int r0 = (298 * y0 + 409 * v + 128) >> 8;
        int g0 = (298 * y0 - 100 * u - 208 * v + 128) >> 8;
        int b0 = (298 * y0 + 516 * u + 128) >> 8;

        // Convert the second pixel (Y1, U, V)
        int r1 = (298 * y1 + 409 * v + 128) >> 8;
        int g1 = (298 * y1 - 100 * u - 208 * v + 128) >> 8;
        int b1 = (298 * y1 + 516 * u + 128) >> 8;

        // Clamp values to valid RGB range (0-255)
        rgb_image[j]     = r0 > 255 ? 255 : (r0 < 0 ? 0 : r0); // Red 0
        rgb_image[j + 1] = g0 > 255 ? 255 : (g0 < 0 ? 0 : g0); // Green 0
        rgb_image[j + 2] = b0 > 255 ? 255 : (b0 < 0 ? 0 : b0); // Blue 0

        rgb_image[j + 3] = r1 > 255 ? 255 : (r1 < 0 ? 0 : r1); // Red 1
        rgb_image[j + 4] = g1 > 255 ? 255 : (g1 < 0 ? 0 : g1); // Green 1
        rgb_image[j + 5] = b1 > 255 ? 255 : (b1 < 0 ? 0 : b1); // Blue 1
    }

    return rgb_image;
}