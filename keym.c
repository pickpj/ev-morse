#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/time.h>

int main(int argc, char **argv)
{
    int fd;
    struct input_event ev;

    if (argc < 2) {
        printf("Usage: %s <input device>\n", argv[0]);
        return 1;
    }

    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("could not open device");
        return 1;
    }

    struct timeval start_time, end_time;
    while (1) {
        char pattern[100] = "";
        int hold = 0;
        while (1) {
            if (read(fd, &ev, sizeof(ev)) < sizeof(ev)) {
                perror("error reading");
                break;
            }

            if (ev.type == EV_KEY && ev.code > 160) {
                int lindx = strlen(pattern) - 1;
                if (hold < 1 &&ev.value == 2) {
                    strcat(pattern,"1");
                    hold++;
                    gettimeofday(&end_time, NULL);
                    double elapsed_time = (end_time.tv_sec - start_time.tv_sec) \
                                        + (end_time.tv_usec - start_time.tv_usec) / 1e6;
                    printf("\nKey code: %d, Value: %s time since last hold: %.6f ", \
                            ev.code, pattern, elapsed_time);
                    gettimeofday(&start_time, NULL);
                } else if (ev.value ==2) {
                } else if (hold < 1 && ev.value == 0) {
                    printf("\nKey code: %d, Value: %s", ev.code, pattern);
                    strcat(pattern,"0");
                    hold = 0;
                } else if (ev.value == 1) {
                    printf("\n%d", ev.value);
                    hold = 0;
                } else {
                    printf("\n%d", ev.value);
                }

            }
        }
    }

    close(fd);
    return 0;
}