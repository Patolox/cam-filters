#include <linux/videodev2.h> // V4L2 definitions
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/ioctl.h> // For IO control operations (ioctl)
#include <errno.h> // Error handling
#include <fcntl.h>  // For file control operations (open, close)
#include <unistd.h> // For close
#include <sys/mman.h> // Memory mapping (for capturing)

int query_cap(int fd);
int set_fmt(int fd, int width, int height);
int req_buffer(int fd);
int queue_buffers(int fd, struct v4l2_buffer* buf, int buffer_count, void** buffer_start);
uint8_t* yuv_to_rgb(void* buffer_start, int width, int height);