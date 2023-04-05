#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <signal.h>
#include <regex.h>

#define ANSI_RED     "\x1b[31m"
#define ANSI_RESET   "\x1b[0m"

char pattern[100] = "";
int keyc = -1;

char grave[10] = "";

FILE* fp;
char mpos[50];
regex_t regex;
int reti;
const char *mregex = "x:([0-9]+) y:([0-9]+)";
size_t nmatch = 3;
regmatch_t pmatch[3];

static void run_hold() {
    int len = strlen(pattern);
    // vvv Your code goes here vvv
    switch (keyc) {
        case 34:
            system("playerctl --player=spotify,chromium,cmus,%%any play-pause");
            break;
    }
    // ^^^ Your code goes here ^^^
    memset(pattern, 0, sizeof(pattern));
}

static void run_pattern() {
    int len = strlen(pattern);
    // vvv Your code goes here vvv
    switch (keyc) {
        case 34:
            if (strcmp(pattern,"0") == 0){
                system("playerctl --player=spotify,chromium,cmus,%%any previous");
            } else if (strcmp(pattern,"1") == 0) {
                system("playerctl --player=spotify,chromium,cmus,%%any next");
            } else {
                system("playerctl --player=spotify,chromium,cmus,%%any position 3+");
            }
            break;
    }
    // ^^^ Your code goes here ^^^
    memset(pattern, 0, sizeof(pattern));
}


int main(int argc, char **argv)
{
    int fd;
    struct input_event ev;
    if (argc < 2) {
        printf(ANSI_RED "Usage: " ANSI_RESET "sudo %s <input device> \n", argv[0]);
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
    timer.it_value.tv_usec = 300000;

    // Timing between key held action
    int interval = 10;

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
            
            if (ev.value == 2) {
                if (hold < 1){
                    strcat(pattern,"1");
                    hold++;
                } else if (hold % interval == (interval - 1)) {
                    memset(pattern, 0, sizeof(pattern));
                    run_hold();
                    hold ++;
                } else {
                    hold ++;
                }
            } else if (ev.value == 1) {
                hold = 0;
            } else if (ev.value == 0) {
                if (hold > interval) {
                    hold = 0;
                    printf("Key code: %d, Value: %s \n", ev.code, pattern);
                    fflush(stdout);
                    memset(pattern, 0, sizeof(pattern));
                } else if (hold > 0) {
                    hold = 0;
                    printf("Key code: %d, Value: %s \n", ev.code, pattern);
                    fflush(stdout);
                } else {
                    strcat(pattern,"0");
                    printf("Key code: %d, Value: %s \n", ev.code, pattern);
                    fflush(stdout);
                }
            }
        
            setitimer(ITIMER_REAL, &timer, NULL);
        }
    }
    close(fd);
    return 0;
}

