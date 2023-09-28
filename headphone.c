#include <linux/input-event-codes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <libevdev/libevdev.h>
#include <libevdev/libevdev-uinput.h>

#define ANSI_RED     "\x1b[31m"
#define ANSI_GREEN   "\x1b[32m"
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
int arguid;

static void evdev_write(int keyp);
static void run_non_alrm(int uid, char *func);

// User defined functions run_hold and run_pattern
static void run_down() {
    switch (keyc) {
        case 200:
        case 201:
            system("playerctl --player=vlc,spotify,youtube-music,cmus,%%any play-pause");
            break;
        case 163:
            system("playerctl --player=vlc,spotify,youtube-music,cmus,%%any next");
            break;
        case 165:
            system("playerctl --player=vlc,spotify,youtube-music,cmus,%%any previous");
            break;
    }
}

static void run_hold() {
    int len = strlen(pattern);
    //! vvv Your code goes here vvv
    switch (keyc) {
    }
    //! ^^^ Your code goes here ^^^
}

static void run_pattern() {
    int status;
    int child = fork();
    if (child == 0) {
        if (setuid(arguid) == -1) {
            perror("setuid");
            _exit(1);
        }
        int len = strlen(pattern);
        if (len >= 1) {
        //! vvv Your code goes here vvv
            switch (keyc) {
            }
        //! ^^^ Your code goes here ^^^
        }
        kill(child, SIGTERM);
        return;
    } else if (child > 0) {
        memset(pattern, 0, sizeof(pattern));
        waitpid(child,&status,WUNTRACED | WCONTINUED);
        return;
    } else {
        perror("fork");
        _exit(1);
    }
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

    // Changing uid so that commands execute as the user (defined by '$(id -u)' ; important for interactions with playerctl)
    // Could be removed to run all commands as sudo
    arguid = atoi(argv[1]);

    // Creates a "fake" device that sends 2 numlock key presses
    // This is to determine whether numlock is on/off
    int err;
    struct libevdev *dev;
    dev = libevdev_new ();
    libevdev_set_name (dev, "ev-headphone");

    libevdev_enable_event_type(dev, EV_KEY);

    libevdev_enable_event_code(dev, EV_KEY, KEY_VOLUMEUP, NULL);
    libevdev_enable_event_code(dev, EV_KEY, KEY_VOLUMEDOWN, NULL);


    err = libevdev_uinput_create_from_device (dev, LIBEVDEV_UINPUT_OPEN_MANAGED, &uidev);
    if (err != 0) {
        return err;
    }

    // Device is destroyed upon sigint signal, for those who would like to the virtual keyboard to send input
    // This method can be faster than xdotool for more niche key characters
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
                run_non_alrm(arguid, "down");
            } else {
                keyc = ev.code;
                // value 2; key held down
                if (ev.value == 2) {
                    if (hold < 1){
                        strcat(pattern,"1");
                        hold++;
                    } else if ((hold-5) % interval == (interval - 1)) {
                        run_non_alrm(arguid, "hold");
                        hold ++;
                    } else {
                        hold ++;
                    }
                // value 1; key down
                } else if (ev.value == 1) {
                    hold = 0;
                    run_non_alrm(arguid, "down");
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
        } else if (access(argv[2], F_OK) != 0) {
            printf(ANSI_RED "Lost device: " ANSI_RESET "waiting for reconnect\n");
            fflush(stdout);
            while (access(argv[2], F_OK) != 0) {
                usleep(500000); //0.5s
            }

            fd = open(argv[2], O_RDONLY);
            if (fd < 0) {
                perror("Failed to reopen device");
                return 1;
            }
            printf(ANSI_GREEN "Device reconnected\n" ANSI_RESET);
            fflush(stdout);
        }
    }
}

// Extra functions

static void run_non_alrm(int uid, char *func) {
    int status;
    int child = fork();
    if (child == 0) {
        if (setuid(uid) == -1) {
            perror("setuid");
            exit(1);
        }
        if (!strcmp(func, "hold")) {
            run_hold();
            memset(pattern, 0, sizeof(pattern));
        } else if (!strcmp(func, "down")) {
            run_down();
        } else {
            perror("run_non_alrm");
        }
        kill(child, SIGTERM);
        return;
    } else if (child > 0) {
        if (!strcmp(func, "hold")) {
            memset(pattern, 0, sizeof(pattern));            
        }
        waitpid(child,&status,WUNTRACED | WCONTINUED | WIFSIGNALED(status));
        return;
    } else {
        perror("fork");
        exit(1);
    }
}

static void evdev_write(int keyp) {
    libevdev_uinput_write_event (uidev, EV_KEY, keyp, 1);
    libevdev_uinput_write_event (uidev, EV_SYN, SYN_REPORT, 0);
    libevdev_uinput_write_event (uidev, EV_KEY, keyp, 0);
    libevdev_uinput_write_event (uidev, EV_SYN, SYN_REPORT, 0);
}

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
