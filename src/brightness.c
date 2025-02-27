// brightness.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "brightness.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int get_max_brightness() {
    FILE *file = fopen(MAX_BRIGHTNESS_PATH, "r");
    if (file == NULL) {
        perror("Failed to open max brightness file");
        exit(EXIT_FAILURE);
    }
    int max_brightness;
    if (fscanf(file, "%d", &max_brightness) != 1) {
        perror("Failed to read max brightness value");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    fclose(file);
    return max_brightness;
}

int get_current_brightness() {
    FILE *file = fopen(BRIGHTNESS_PATH, "r");
    if (file == NULL) {
        perror("Failed to open brightness file");
        exit(EXIT_FAILURE);
    }
    int brightness;
    if (fscanf(file, "%d", &brightness) != 1) {
        perror("Failed to read brightness value");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    fclose(file);
    return brightness;
}

void set_brightness(int value, int raw, int no_display) {
    int max_brightness = get_max_brightness();
    if (!raw) {
        value = (value * max_brightness) / 100;
    }
    FILE *brightness_file = fopen(BRIGHTNESS_PATH, "w");
    if (brightness_file == NULL) {
        perror("Failed to open brightness file for writing");
        exit(EXIT_FAILURE);
    }
    if (fprintf(brightness_file, "%d", value) < 0) {
        perror("Failed to write brightness value");
        fclose(brightness_file);
        exit(EXIT_FAILURE);
    }
    fclose(brightness_file);
    if (!no_display) {
        display_brightness(value);
    }
}

void adjust_brightness(const char *direction, int no_display) {
    int brightness = get_current_brightness();
    int max_brightness = get_max_brightness();

    if (strcmp(direction, "up") == 0) {
        brightness += max_brightness / 10;
    } else if (strcmp(direction, "down") == 0) {
        brightness -= max_brightness / 10;
    } else {
        fprintf(stderr, "Error: Invalid adjust direction. Use 'up' or 'down'.\n");
        exit(EXIT_FAILURE);
    }

    if (brightness < 0) {
        brightness = 0;
    } else if (brightness > max_brightness) {
        brightness = max_brightness;
    }

    set_brightness(brightness, 1, no_display);
}

void draw_icon(Display *d, Window w, GC gc) {
    int circle_radius = 22;     // Radius for a smaller circle
    int ray_length = 27;        // Length of the rays
    int line_thickness = 5;     // Increased line thickness
    int y_offset = 10;          // Amount to move the icon down

    // Set the line attributes to have thicker lines with rounded edges
    XSetLineAttributes(d, gc, line_thickness, LineSolid, CapRound, JoinRound);

    // Draw the smaller center circle, moved down by y_offset
    XDrawArc(d, w, gc, 100 - circle_radius, 40 - circle_radius + y_offset, 2 * circle_radius, 2 * circle_radius, 0, 360 * 64);

    // Draw the rays with adjusted length, moved down by y_offset
    for (int i = 0; i < 8; ++i) {
        double angle = i * M_PI / 4.0;
        int x1 = 100 + (int)((circle_radius + 5) * cos(angle)); // Start just outside the circle
        int y1 = 40 + (int)((circle_radius + 5) * sin(angle)) + y_offset;
        int x2 = 100 + (int)((circle_radius + ray_length) * cos(angle)); // Extend to ray_length
        int y2 = 40 + (int)((circle_radius + ray_length) * sin(angle)) + y_offset;
        XDrawLine(d, w, gc, x1, y1, x2, y2);
    }
}

void display_brightness(int brightness) {
    Display *d;
    Window w;
    XEvent e;
    int screen;
    unsigned int display_width, display_height;
    int width = 200, height = 120;
    int sections = 10;
    int graph_height = 10;

    if ((d = XOpenDisplay(NULL)) == NULL) {
        fprintf(stderr, "Cannot open display\n");
        exit(EXIT_FAILURE);
    }

    screen = DefaultScreen(d);
    display_width = DisplayWidth(d, screen);
    display_height = DisplayHeight(d, screen);

    w = XCreateSimpleWindow(d, RootWindow(d, screen), (display_width - width) / 2, (display_height - height) / 2, width, height, 1,
                            BlackPixel(d, screen), WhitePixel(d, screen));
    XSetWindowBackground(d, w, 0xD3D3D3); // Light gray
    XStoreName(d, w, "BrightnessControl");
    XClassHint *classHint = XAllocClassHint();
    if (classHint) {
        classHint->res_name = "brightnesscontrol";
        classHint->res_class = "BrightnessControl";
        XSetClassHint(d, w, classHint);
        XFree(classHint);
    }

    XSelectInput(d, w, ExposureMask | KeyPressMask | StructureNotifyMask);
    XMapWindow(d, w);

    GC gc = XCreateGC(d, w, 0, NULL);
    GC bg_gc = XCreateGC(d, w, 0, NULL);
    XSetForeground(d, gc, BlackPixel(d, screen));
    XSetBackground(d, bg_gc, WhitePixel(d, screen));
    XSetLineAttributes(d, gc, 3, LineSolid, CapButt, JoinMiter);

    int max_brightness = get_max_brightness();

    // Wait for the window to be mapped
    while (1) {
        XNextEvent(d, &e);
        if (e.type == MapNotify) {
            break;
        }
    }

    // Draw the icon
    draw_icon(d, w, gc);

    // Draw the background for graph
    XSetForeground(d, bg_gc, 0xA9A9A9); // Dark gray
    XFillRectangle(d, w, bg_gc, 10, 100, 180, graph_height);

    // Draw the graph
    int filled_sections = (int)((brightness / (double)max_brightness) * sections);
    for (int i = 0; i < sections; ++i) {
        if (i < filled_sections) {
            XSetForeground(d, gc, 0xFFFFFF); // White
        } else {
            XSetForeground(d, gc, 0xA9A9A9); // Dark gray
        }
        XFillRectangle(d, w, gc, 10 + i * (180 / sections), 100, (180 / sections) - 2, graph_height);
    }

    XFlush(d);
    sleep(1); // Display the window for a brief period
    XDestroyWindow(d, w);
    XCloseDisplay(d);
}
