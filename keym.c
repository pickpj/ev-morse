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
        perror("Failed to open device");
        return 1;
    }
    struct timeval start_time, end_time;
    int keyc = -1;
    int hold = 0;
    char pattern[100] = "";
    int lol = 500;
    while (1) {
        if (read(fd, &ev, sizeof(ev)) < sizeof(ev)) {
            perror("Error reading");
            break;
        }
        if (ev.type == EV_KEY) {
            if (keyc != ev.code && keyc != -1) {
                keyc = ev.code;
                printf("\n evcode %d \n keyc %d", ev.code, keyc);
                fflush(stdout);
                memset(pattern, 0, sizeof(pattern));
            }
            if (ev.code > 160) {
                keyc = ev.code;
                int lindx = strlen(pattern) - 1;
                if (hold < 1 && ev.value == 2) {
                    strcat(pattern,"1");
                    hold++;
                } else if (ev.value == 0 && hold > 0) {
                    hold = 0;
                    gettimeofday(&end_time, NULL);
                    double elapsed_time = (end_time.tv_sec - start_time.tv_sec) \
                                        + (end_time.tv_usec - start_time.tv_usec) / 1e6;
                    printf("\nKey code: %d, Value: %s time since last hold: %.6f ", \
                            ev.code, pattern, elapsed_time);
                    fflush(stdout);
                    gettimeofday(&start_time, NULL);
                } else if (hold < 1 && ev.value == 0) {
                    strcat(pattern,"0");
                    printf("\nKey code: %d, Value: %s", ev.code, pattern);
                    fflush(stdout);
                } else if (ev.value == 1) {
                    hold = 0;
                }
            }
        }
    }

    close(fd);
    return 0;
}