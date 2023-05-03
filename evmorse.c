#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
// #include <linux/input.h>
#include <linux/uinput.h>

#define ANSI_RED     "\x1b[31m"
#define ANSI_RESET   "\x1b[0m"

// main variables
char pattern[100] = "";
int keyc = -1;
int nlstate;
int shiftstate = 0;
int ctrlstate = 0;
int altstate = 0;


// User defined functions run_hold and run_pattern

static void run_hold() {
    int len = strlen(pattern);
    // vvv Your code goes here vvv
    switch (keyc) {
        case 164:
            system("playerctl --player=spotify,youtube-music,cmus,%%any play-pause");
            memset(pattern, 0, sizeof(pattern));
            break;
        case 163:
            system("playerctl --player=spotify,youtube-music,cmus,%%any position 5+");
            memset(pattern, 0, sizeof(pattern));
            break;
        case 165:
            system("playerctl --player=spotify,youtube-music,cmus,%%any position 5-");
            memset(pattern, 0, sizeof(pattern));
            break;
    }
    // ^^^ Your code goes here ^^^
}

static void run_pattern() {
    int len = strlen(pattern);
    if (len >= 1) {
    // vvv Your code goes here vvv
        switch (keyc) {
        case 96:
            break;
        case 164:
            system("playerctl --player=spotify,youtube-music,cmus,%%any play-pause");
            break;
        case 163:
            for(int i = 0; i < len; i++) {
                system("playerctl --player=spotify,youtube-music,cmus,%%any next");
            } 
            break;
        case 165:
            for(int i = 0; i < len; i++) {
                system("playerctl --player=spotify,youtube-music,cmus,%%any previous");
            } 
            break;
        case 74:
            if (strcmp(pattern,"0") == 0){
                system("playerctl -p spotify volume 0.5 || playerctl -p youtube-music volume 5 || playerctl --player=cmus,%%any volume 0.5");
            } else if (strcmp(pattern,"1") == 0) {
                system("playerctl -p spotify volume 0.7 || playerctl -p youtube-music volume 20 || playerctl --player=cmus,%%any volume 0.7");
            } else if (len == 2){
                system("playerctl -p spotify volume 1 || playerctl -p youtube-music volume 50 || playerctl --player=cmus,%%any volume 1");
            } else {
                system("playerctl -p spotify volume 1 || playerctl -p youtube-music volume 100 || playerctl --player=cmus,%%any volume 1");
            }
            break;
        }
    }
    // ^^^ Your code goes here ^^^
    memset(pattern, 0, sizeof(pattern));
}

// main function declarations 
static void emit(int fd, int type, int code, int val);
static void setmodifier(int keycode, int value);

int main(int argc, char **argv)
{
    int fd;
    struct input_event ev;
    if (argc < 3) {
        printf(ANSI_RED "Usage: " ANSI_RESET "sudo %s $(id -u) <input device>\n", argv[0]);
        return 1;
    }
    fd = open(argv[2], O_RDONLY);
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
    int argid = atoi(argv[1]);
    setuid(argid);
    uid_t uid = getuid();
    sprintf(dbus_addr, "unix:path=/run/user/%d/bus", uid);
    setenv("DBUS_SESSION_BUS_ADDRESS", dbus_addr, 1);

    
    fd_set readfds;
    struct timeval timeout;
    struct uinput_setup usetup;
    int fd2 = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

    ioctl(fd2, UI_SET_EVBIT, EV_KEY);
    ioctl(fd2, UI_SET_KEYBIT, KEY_NUMLOCK);

    memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_USB;
    strcpy(usetup.name, "ev-morse numlock");

    ioctl(fd2, UI_DEV_SETUP, &usetup);
    ioctl(fd2, UI_DEV_CREATE);
    sleep(1);    

    emit(fd2, EV_KEY, KEY_NUMLOCK, 1);
    emit(fd2, EV_SYN, SYN_REPORT, 0);
    emit(fd2, EV_KEY, KEY_NUMLOCK, 0);
    emit(fd2, EV_SYN, SYN_REPORT, 0);

    while (1) {
        struct input_event ev;
        int n = read(fd, &ev, sizeof(ev));
        if (ev.type == EV_LED) {
            printf("LED event: code=%d, value=%d\n", ev.code, ev.value);
            fflush(stdout);
            emit(fd2, EV_KEY, KEY_NUMLOCK, 1);
            emit(fd2, EV_SYN, SYN_REPORT, 0);
            emit(fd2, EV_KEY, KEY_NUMLOCK, 0);
            emit(fd2, EV_SYN, SYN_REPORT, 0);
            nlstate = !ev.value;
            printf("LED event: code=%d, value=%d\n", ev.code, !ev.value);
            break;
        }
    }
    ioctl(fd2, UI_DEV_DESTROY);
    close(fd2);


    while (1) {
        if (read(fd, &ev, sizeof(ev)) < sizeof(ev)) {
            perror("Error reading");
            break;
        }
        if (ev.type == EV_KEY) {
            if (ev.code == 97 | ev.code == 29 | ev.code == 54 | ev.code == 42 | ev.code == 100 | ev.code == 56) {
                if (ev.value != 2) {
                    setmodifier(ev.code, ev.value);
                }
            } else if (keyc != ev.code && keyc != -1) {
                memset(pattern, 0, sizeof(pattern));
                keyc = ev.code;
            } else {
                keyc = ev.code;
                if (ev.value == 2) {
                    if (hold < 1){
                        strcat(pattern,"1");
                        hold++;
                    } else if (hold % interval == (interval - 1)) {
                        run_hold();
                        hold ++;
                    } else {
                        hold ++;
                    }
                } else if (ev.value == 1) {
                    hold = 0;
                } else if (ev.value == 0) {
                    if (keyc == 69) {
                        nlstate = !nlstate;
                        printf("Numlock State %d \n", nlstate);
                    }
                    if (strlen(pattern) == 0 && hold > interval) {
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
    }
    close(fd);
    return 0;
}

static void setmodifier(int keycode, int value) {
    switch (keycode) {
        case 42: //lshift
        case 54: //rshift
            if (value == 1) {
                shiftstate = 1;
                printf("shift on\n");
                fflush(stdout);
            } else {
                shiftstate = 0;
                printf("shift off\n");
                fflush(stdout);
            }
            break;
        case 29: //lctrl
        case 97: //rctrl
            if (value == 1) {
                ctrlstate = 1;
                printf("ctrl on\n");
                fflush(stdout);
            } else {
                ctrlstate = 0;
                printf("ctrl off\n");
                fflush(stdout);
            }
            break;
        case 56: //lalt
        case 100: //ralt
            if (value == 1) {
                altstate = 1;
                printf("alt on\n");
                fflush(stdout);
            } else {
                altstate = 0;
                printf("alt off\n");
                fflush(stdout);
            }
            break;
    }
}

// Functions used by the main loop
static void emit(int fd, int type, int code, int val) {
    struct input_event ie;
    ie.type = type;
    ie.code = code;
    ie.value = val;

    write(fd, &ie, sizeof(ie));
}
