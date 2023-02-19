#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <signal.h>

char pattern[100] = "";
int keyc = -1;

void run_pattern(int sig) {
    int len = strlen(pattern);
    // vvv Your code goes here vvv
    if (keyc == 164){
        system("playerctl play-pause");
    } else if (keyc == 165) {
        if (strcmp(pattern, "01") == 0){
            system("playerctl next");
        } else if (strcmp(pattern, "00") == 0){
            system("playerctl position 10+");
        }
    } else if (keyc == 163) {
        for(int i = 0; i < len; i++) {
            system("");
        }
    }
    // ^^^ Your code goes here ^^^
    memset(pattern, 0, sizeof(pattern));
}

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
    int hold = 0;

    // ADJUST TIMINGS HERE
    struct itimerval timer;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 350000;

    signal(SIGALRM, run_pattern);
    
    char dbus_addr[50];
    setuid(1000);
    uid_t uid = getuid();
    sprintf(dbus_addr, "unix:path=/run/user/%d/bus", uid);
    setenv("DBUS_SESSION_BUS_ADDRESS", dbus_addr, 1);

    while (1) {
        if (read(fd, &ev, sizeof(ev)) < sizeof(ev)) {
            perror("Error reading");
            break;
        }
        if (ev.type == EV_KEY) {
            if (keyc != ev.code && keyc != -1) {
                memset(pattern, 0, sizeof(pattern));
            }
            keyc = ev.code;
            int lindx = strlen(pattern) - 1;
            
            if (hold < 1 && ev.value == 2) {
                strcat(pattern,"1");
                hold++;
            } else if (ev.value == 0 && hold > 0) {
                hold = 0;
                printf("\nKey code: %d, Value: %s", ev.code, pattern);
                fflush(stdout);
            } else if (hold < 1 && ev.value == 0) {
                strcat(pattern,"0");
                printf("\nKey code: %d, Value: %s", ev.code, pattern);
                fflush(stdout);
            } else if (ev.value == 1) {
                hold = 0;
            }
            setitimer(ITIMER_REAL, &timer, NULL);
        }
    }

    close(fd);
    return 0;
}