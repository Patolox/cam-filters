# cam-filters

**cam-filters** is a C application that uses SDL2 and V4L2 to display webcam video and apply various image filters in real time.

## Features

The following filters can be applied to the live webcam feed:

- **Sepia Filter** (`s`)
- **Reflection** (`r`)
- **Blur Filter** (`b`)
- **Grayscale Filter** (`g`)
- **Edge Detection** (`e`)
- **Dot Matrix Effect** (`d`)

## Requirements

To compile and run this application, you will need:

- [SDL2](https://www.libsdl.org/) for video rendering.
- [V4L2](https://linuxtv.org/downloads/v4l-dvb-apis-new/uapi/v4l2.html) for webcam access.

Use the following command to install these dependencies if you are in Ubuntu Linux:

```bash
make dep
```
  
Make sure you have these libraries installed on your system.

## Usage

After compiling the application, run the executable. You can apply the filters in real time by pressing the corresponding keys on your keyboard:

- Press `s` to apply the **Sepia** filter.
- Press `r` to **Reflect** the webcam video horizontally.
- Press `b` to apply a **Blur** filter.
- Press `g` to convert the video to **Grayscale**.
- Press `e` to apply **Edge Detection**.
- Press `d` to activate the **Dot Matrix** effect.

## Compilation

To compile the application, use the following command:

```bash
make cam
```
