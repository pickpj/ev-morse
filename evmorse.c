#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <libevdev/libevdev.h>
#include <libevdev/libevdev-uinput.h>

#define ANSI_RED     "\x1b[31m"
#define ANSI_RESET   "\x1b[0m"

// main variables
char pattern[100] = "";
int keyc = -1;
int nlstate;
int shiftstate = 0;
int ctrlstate = 0;
int altstate = 0;

int fd;
struct libevdev_uinput *uidev;

// User defined functions run_hold and run_pattern
static void run_hold() {
    int len = strlen(pattern);
    //! vvv Your code goes here vvv
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
    //! ^^^ Your code goes here ^^^
}

static void run_pattern() {
    int len = strlen(pattern);
    if (len >= 1) {
    //! vvv Your code goes here vvv
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
    //! ^^^ Your code goes here ^^^
    memset(pattern, 0, sizeof(pattern));
}


// Function declarations for main (Found near the end)
static void setmodifier(int keycode, int value);
static void exitsignal();

int main(int argc, char **argv)
{
    // Check for proper input of command
    struct input_event ev;
    if (argc < 3) {
        printf(ANSI_RED "Usage: " ANSI_RESET "sudo -E %s $(id -u) <input device>\n", argv[0]);
        return 1;
    }
    fd = open(argv[2], O_RDONLY);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }
    int hold = 0;

    //^ ADJUST TIMINGS HERE
    struct itimerval timer;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 300000;

    //^ Timing between key held action (30ms * interval)
    int interval = 10;

    // Set signal to run the function: run_pattern
    signal(SIGALRM, run_pattern);
    // Set signal for exiting the program (ctrl+c)
    signal(SIGINT, exitsignal);

    // Changing dbus so that commands execute as the user (defined by '$(id -u)' ; important for interactions with playerctl)
    // Could be removed to run all commands as sudo
    char dbus_addr[50];
    int argid = atoi(argv[1]);
    setuid(argid);
    uid_t uid = getuid();
    sprintf(dbus_addr, "unix:path=/run/user/%d/bus", uid);
    setenv("DBUS_SESSION_BUS_ADDRESS", dbus_addr, 1);

    // Creates a "fake" device that sends 2 numlock key presses
    // This is to determine whether numlock is on/off
    int err;
    struct libevdev *dev;
    dev = libevdev_new ();
    libevdev_set_name (dev, "ev-morse numlock");

    // This device is only able to send a numlock key
    libevdev_enable_event_type (dev, EV_KEY);
    libevdev_enable_event_code (dev, EV_KEY, KEY_NUMLOCK, NULL);

    err = libevdev_uinput_create_from_device (dev, LIBEVDEV_UINPUT_OPEN_MANAGED, &uidev);
    if (err != 0) {
        return err;
    }
    sleep(1);    

    // Send Numlock
    libevdev_uinput_write_event (uidev, EV_KEY, KEY_NUMLOCK, 1);
    libevdev_uinput_write_event (uidev, EV_SYN, SYN_REPORT, 0);
    libevdev_uinput_write_event (uidev, EV_KEY, KEY_NUMLOCK, 0);
    libevdev_uinput_write_event (uidev, EV_SYN, SYN_REPORT, 0);

    // Listen and Send Numlock upon LED event
    while (1) {
        struct input_event ev;
        int n = read(fd, &ev, sizeof(ev));
        if (ev.type == EV_LED) {
            printf("LED event: code=%d, value=%d\n", ev.code, ev.value);
            fflush(stdout);
            libevdev_uinput_write_event (uidev, EV_KEY, KEY_NUMLOCK, 1);
            libevdev_uinput_write_event (uidev, EV_SYN, SYN_REPORT, 0);
            libevdev_uinput_write_event (uidev, EV_KEY, KEY_NUMLOCK, 0);
            libevdev_uinput_write_event (uidev, EV_SYN, SYN_REPORT, 0);
            nlstate = !ev.value;
            printf("LED event: code=%d, value=%d\n", ev.code, !ev.value);
            break;
        }
    }
    // Device is destroyed upon sigint signal instead of here for those 
    // who would like to send keystrokes via the virtual keyboard.
    // This method can be faster than xdotool for more niche key characters.
    // Device can be destroyed here if desired, but comment out the same line in the exitsignal() function

    // libevdev_uinput_destroy (uidev);

    while (1) {
        if (read(fd, &ev, sizeof(ev)) < sizeof(ev)) {
            perror("Error reading");
            break;
        }
        if (ev.type == EV_KEY) {
            // Checks for modifier keys
            if (ev.code == 97 | ev.code == 29 | ev.code == 54 | ev.code == 42 | ev.code == 100 | ev.code == 56) {
                if (ev.value != 2) {
                    setmodifier(ev.code, ev.value);
                }
            // Checks whether the key has changed
            } else if (keyc != ev.code && keyc != -1) {
                memset(pattern, 0, sizeof(pattern));
                keyc = ev.code;
            } else {
                keyc = ev.code;
                // value 2; key held down
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
                // value 1; key down
                } else if (ev.value == 1) {
                    hold = 0;
                // value 0; key up
                } else if (ev.value == 0) {
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
            // Set / Reset timer to trigger the signal (run_pattern)
            setitimer(ITIMER_REAL, &timer, NULL);
            }
        } else if (ev.type == EV_LED) {
            if (ev.code == 0) {
                nlstate = ev.value;
                printf("Numlock State %d\n",nlstate);
                fflush(stdout);
            }
        }
    }
}


// Functions used by the main loop
static void setmodifier(int keycode, int value) {
    switch (keycode) {
        case 42: //lshift
        case 54: //rshift
            shiftstate = value;
            printf("Shift State %d\n",shiftstate);
            fflush(stdout);
            break;
        case 29: //lctrl
        case 97: //rctrl
            ctrlstate = value;
            printf("Ctrl State %d\n",ctrlstate);
            fflush(stdout);
            break;
        case 56: //lalt
        case 100: //ralt
            altstate = value;
            printf("Alt State %d\n",altstate);
            fflush(stdout);
            break;
    }
}

static void exitsignal() {
    printf("SIGINT signal recieved");
    libevdev_uinput_destroy (uidev);
    close(fd);
    exit(0);
}
