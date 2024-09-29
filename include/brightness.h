// brightness.h
#ifndef BRIGHTNESS_H
#define BRIGHTNESS_H

#define BRIGHTNESS_PATH "/sys/class/backlight/intel_backlight/brightness"
#define MAX_BRIGHTNESS_PATH "/sys/class/backlight/intel_backlight/max_brightness"

int get_max_brightness();
int get_current_brightness();
void set_brightness(int value, int raw, int no_display);
void adjust_brightness(const char *direction, int no_display);
void display_brightness(int brightness);

#endif // BRIGHTNESS_H
