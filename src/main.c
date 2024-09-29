// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "brightness.h"

void print_usage(const char *prog_name) {
    fprintf(stderr, "Usage: %s [--machine-readable|-m] [--human-readable|-hr] [--adjust|-a up|down] [--set|-s <value>] [--nodisplay|-nd]\n", prog_name);
}

int main(int argc, char *argv[]) {
    int show_machine_brightness = 0;
    int show_human_brightness = 0;
    const char *adjust_direction = NULL;
    int set_brightness_value = -1;
    int set_brightness_raw = 0;
    int no_display = 0; // New flag for --nodisplay

    if (argc < 2) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--machine-readable") == 0 || strcmp(argv[i], "-m") == 0) {
            show_machine_brightness = 1;
        } else if (strcmp(argv[i], "--human-readable") == 0 || strcmp(argv[i], "-hr") == 0) {
            show_human_brightness = 1;
        } else if (strcmp(argv[i], "--adjust") == 0 || strcmp(argv[i], "-a") == 0) {
            if (i + 1 < argc) {
                adjust_direction = argv[++i];
            } else {
                print_usage(argv[0]);
                return EXIT_FAILURE;
            }
        } else if (strcmp(argv[i], "--set") == 0 || strcmp(argv[i], "-s") == 0) {
            if (i + 1 < argc) {
                set_brightness_value = atoi(argv[++i]);
                if (set_brightness_value < 0 || set_brightness_value > 100) {
                    fprintf(stderr, "Error: Value for --set must be between 0 and 100\n");
                    return EXIT_FAILURE;
                }
            } else {
                print_usage(argv[0]);
                return EXIT_FAILURE;
            }
        } else if (strcmp(argv[i], "--nodisplay") == 0 || strcmp(argv[i], "-nd") == 0) {
            no_display = 1; // Set the no_display flag
        } else {
            print_usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    if (show_machine_brightness) {
        int brightness = get_current_brightness();
        printf("Current brightness (machine-readable): %d\n", brightness);
        return EXIT_SUCCESS;
    }

    if (show_human_brightness) {
        int brightness = get_current_brightness();
        int max_brightness = get_max_brightness();
        printf("Current brightness (human-readable): %d%%\n", (brightness * 100) / max_brightness);
        return EXIT_SUCCESS;
    }

    if (adjust_direction) {
        adjust_brightness(adjust_direction, no_display);
        return EXIT_SUCCESS;
    }

    if (set_brightness_value != -1) {
        set_brightness(set_brightness_value, set_brightness_raw, no_display);
        return EXIT_SUCCESS;
    }

    print_usage(argv[0]);
    return EXIT_FAILURE;
}
