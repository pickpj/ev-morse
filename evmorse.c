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

#include <regex.h>

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


static void evdev_write(int keyp) {
    libevdev_uinput_write_event (uidev, EV_KEY, keyp, 1);
    libevdev_uinput_write_event (uidev, EV_SYN, SYN_REPORT, 0);
    libevdev_uinput_write_event (uidev, EV_KEY, keyp, 0);
    libevdev_uinput_write_event (uidev, EV_SYN, SYN_REPORT, 0);
}

// User defined functions run_hold and run_pattern
static void run_down() {
    switch (keyc) {
        case 55: // *
            if (!nlstate) {
            
            } else {
                evdev_write(KEY_KPASTERISK);
            }
            break;
        case 71: // 7
            if (!nlstate) {
                system("xdotool windowminimize $(xdotool getactivewindow)");
            } else {
                evdev_write(KEY_KP7);
            }
            break;
        case 72: // 8
            if (!nlstate) {

            } else {
                evdev_write(KEY_KP8);
            }
            break;
        case 73: // 9
            if (!nlstate) {
                if (!shiftstate) {
                    system("firefox --private-window ");
                } else {
                    system("tor-browser &");
                }
            } else {
                evdev_write(KEY_KP9);
            }
            break;
        case 74: // -
            if (!nlstate) {

            } else {
                evdev_write(KEY_KPMINUS);
            }
            break;
        case 75: // 4
            if (!nlstate) {
                system("dolphin &");
            } else {
                evdev_write(KEY_KP4);
            }
            break;
        case 76: // 5
            if (!nlstate) {
                if (!shiftstate) {

                } else {
                    evdev_write(KEY_NUMLOCK);
                    system("qalculate-qt &");
                }
            } else {
                evdev_write(KEY_KP5);
            }
            break;
        case 77: // 6
            if (!nlstate) {
                if (!shiftstate&&!altstate&&!ctrlstate) {
                    system("autokey-run -s LCode");
                }
            } else {
                evdev_write(KEY_KP6);
            }
            break;
        case 78: // +
            if (!nlstate) {

            } else {
                evdev_write(KEY_KPPLUS);
            }
            break;
        case 79: // 1
            if (!nlstate) {

            } else {
                evdev_write(KEY_KP1);
            }
            break;
        case 80: // 2
            if (!nlstate) {

            } else {
                evdev_write(KEY_KP2);
            }
            break;
        case 81: // 3
            if (!nlstate) {
                usleep(250000);
                evdev_write(KEY_VOLUMEUP);
            } else {
                evdev_write(KEY_KP3);
            }
            break;
        case 82: // 0
            if (!nlstate) {

            } else {
                evdev_write(KEY_KP0);
            }
            break;
        case 83: // .
            if (!nlstate) {
                // evdev_write(KEY_VOLUMEDOWN);
            } else {
                evdev_write(KEY_KPDOT);
            }
            break;
        case 96: // Enter
            if (!nlstate) {

            } else {
                evdev_write(KEY_KPENTER);
            }
        // case 98: // /
        //     if (!nlstate) {

        //     } else {
        //         libevdev_uinput_write_event (uidev, EV_KEY, KEY_KPSLASH, 1);
        //         libevdev_uinput_write_event (uidev, EV_SYN, SYN_REPORT, 0);
        //         libevdev_uinput_write_event (uidev, EV_KEY, KEY_KPSLASH, 0);
        //         libevdev_uinput_write_event (uidev, EV_SYN, SYN_REPORT, 0);
        //     }
        //     break;
    }
}


static void run_hold() {
    int len = strlen(pattern);
    //! vvv Your code goes here vvv
    switch (keyc) {
        case 164:
            system("playerctl --player=vlc,spotify,youtube-music,cmus,%%any play-pause");
            memset(pattern, 0, sizeof(pattern));
            break;
        case 163:
            system("playerctl --player=vlc,spotify,youtube-music,cmus,%%any position 5+");
            memset(pattern, 0, sizeof(pattern));
            break;
        case 165:
            system("playerctl --player=vlc,spotify,youtube-music,cmus,%%any position 5-");
            memset(pattern, 0, sizeof(pattern));
            break;
        case 81:
            if (nlstate == 0) {
                libevdev_uinput_write_event (uidev, EV_KEY, KEY_VOLUMEUP, 1);
                libevdev_uinput_write_event (uidev, EV_SYN, SYN_REPORT, 0);
                libevdev_uinput_write_event (uidev, EV_KEY, KEY_VOLUMEUP, 0);
                libevdev_uinput_write_event (uidev, EV_SYN, SYN_REPORT, 0);
            } else {
                printf("3");
                fflush(stdout);
            }
            memset(pattern, 0, sizeof(pattern));
            break;
    }
    //! ^^^ Your code goes here ^^^
}

static void run_pattern() {
    int status;
    int child = fork();
    if (child == 0) {
        if (setuid(1000) == -1) {
            perror("setuid");
            exit(1);
        }
        int len = strlen(pattern);
        if (len >= 1) {
        //! vvv Your code goes here vvv
            switch (keyc) {
                case 200:
                case 201:
                    system("playerctl -p spotify play-pause");
                    break;
                case 164:
                    if (!strcmp(pattern, "00")) {
                        system("playerctl -p Blanket play-pause");
                    } else {
                    system("playerctl --player=vlc,spotify,youtube-music,cmus,%%any play-pause");
                    }
                    break;
                case 163:
                    if (!strcmp(pattern, "01")) {
                        system("playerctl --player=vlc,spotify,youtube-music,cmus,%%any loop | grep Track && playerctl --player=vlc,spotify,youtube-music,cmus,%%any loop None || playerctl --player=vlc,spotify,youtube-music,cmus,%%any loop Track");
                    } else {
                        for(int i = 0; i < len; i++) {
                            system("playerctl --player=vlc,spotify,youtube-music,cmus,%%any next");
                        }
                    }
                    break;
                case 165:
                    if (!strcmp(pattern, "01")) {
                        system("playerctl --player=vlc,spotify,youtube-music,cmus,%%any shuffle Toggle");
                    } else {
                        for(int i = 0; i < len; i++) {
                            system("playerctl --player=vlc,spotify,youtube-music,cmus,%%any previous");
                        } 
                    }
                    break;
                case 74:
                    if (!strcmp(pattern,"0")) {
                        system("playerctl --player=vlc,spotify volume 0.5 || playerctl -p youtube-music volume 0.06 || playerctl --player=cmus,%%any volume 0.5");
                    } else if (!strcmp(pattern,"1")) {
                        system("playerctl --player=vlc,spotify volume 0.7 || playerctl -p youtube-music volume 0.2 || playerctl --player=cmus,%%any volume 0.7");
                    } else if (!strcmp(pattern, "00")) {
                        system("playerctl --player=vlc,spotify volume 1 || playerctl -p youtube-music volume 0.49 || playerctl --player=cmus,%%any volume 1");
                    } else if (!strcmp(pattern, "11")) {
                        system("playerctl --player=vlc,spotify volume 0.3 || playerctl -p youtube-music volume 0.02 || playerctl --player=cmus,%%any volume 0.25");
                    } else {
                        system("playerctl --player=vlc,spotify volume 1 || playerctl -p youtube-music volume 1 || playerctl --player=cmus,%%any volume 1");
                    }
                    break;
                case 185:
                    if (!strcmp(pattern, "1")) {
                        system("gnome-pie -o 272");
                    } else {
                        for(int i = 0; i < len; i++) {
                            libevdev_uinput_write_event (uidev, EV_KEY, KEY_GRAVE, 1);
                            libevdev_uinput_write_event (uidev, EV_SYN, SYN_REPORT, 0);
                            libevdev_uinput_write_event (uidev, EV_KEY, KEY_GRAVE, 0);
                            libevdev_uinput_write_event (uidev, EV_SYN, SYN_REPORT, 0);
                        }
                    }
                    break;
                case 81:
                    libevdev_uinput_write_event (uidev, EV_KEY, KEY_VOLUMEUP, 1);
                    libevdev_uinput_write_event (uidev, EV_SYN, SYN_REPORT, 0);
                    libevdev_uinput_write_event (uidev, EV_KEY, KEY_VOLUMEUP, 0);
                    libevdev_uinput_write_event (uidev, EV_SYN, SYN_REPORT, 0);
                    break;
            }
        //! ^^^ Your code goes here ^^^
        }
            kill(child, SIGKILL);
            return;
        } else if (child > 0) {
            memset(pattern, 0, sizeof(pattern));
            waitpid(child,&status,WUNTRACED | WCONTINUED);
            return;
        } else {
        perror("fork");
        exit(1);
    }
}









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
        kill(child, SIGKILL);
        return;
    } else if (child > 0) {
        if (!strcmp(func, "hold")) {
            memset(pattern, 0, sizeof(pattern));            
        }
        waitpid(child,&status,WUNTRACED | WCONTINUED);
        return;
    } else {
        perror("fork");
        exit(1);
    }
}

// int main(int argc, char *argv[]) {
//     char *env[4];
//     if (argc > 1) {
//         printf("usage: %s \n", argv[0]);
//         exit(1);
//     }
//     system("whoami");
//     clock_t begin = clock();
//     run_non_alrm(1000, *argv);
//     clock_t end = clock();
//     double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
//     system("whoami");
//     printf("%f", time_spent);
//     fflush(stdout);
// }











// Function declarations for main (Found near the end)
static void setmodifier(int keycode, int value);
static void exitsignal();

int main(int argc, char **argv)
{
    // Check for proper input of command
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
    int uid = atoi(argv[1]);

    // Creates a "fake" device that sends 2 numlock key presses
    // This is to determine whether numlock is on/off
    int err;
    struct libevdev *dev;
    dev = libevdev_new ();
    libevdev_set_name (dev, "ev-morse numlock");

    // This device is only able to send a numlock key
    libevdev_enable_event_type(dev, EV_KEY);
    libevdev_enable_event_code(dev, EV_KEY, KEY_NUMLOCK, NULL);
    libevdev_enable_event_code(dev, EV_KEY, KEY_A, NULL);
    libevdev_enable_event_code(dev, EV_KEY, KEY_GRAVE, NULL);
    libevdev_enable_event_code(dev, EV_KEY, KEY_LEFTSHIFT, NULL);
    libevdev_enable_event_code(dev, EV_KEY, KEY_VOLUMEUP, NULL);

    err = libevdev_uinput_create_from_device (dev, LIBEVDEV_UINPUT_OPEN_MANAGED, &uidev);
    if (err != 0) {
        return err;
    }
    sleep(1);    

    // Send Numlock
    // libevdev_uinput_write_event (uidev, EV_KEY, KEY_NUMLOCK, 1);
    // libevdev_uinput_write_event (uidev, EV_SYN, SYN_REPORT, 0);
    // libevdev_uinput_write_event (uidev, EV_KEY, KEY_NUMLOCK, 0);
    // libevdev_uinput_write_event (uidev, EV_SYN, SYN_REPORT, 0);

    // Listen and Send Numlock upon LED event
    
    // while (1) {
    //     struct input_event ev;
    //     int n = read(fd, &ev, sizeof(ev));
    //     if (ev.type == EV_LED) {
    //         printf("LED event: code=%d, value=%d\n", ev.code, ev.value);
    //         fflush(stdout);
    //         libevdev_uinput_write_event (uidev, EV_KEY, KEY_NUMLOCK, 1);
    //         libevdev_uinput_write_event (uidev, EV_SYN, SYN_REPORT, 0);
    //         libevdev_uinput_write_event (uidev, EV_KEY, KEY_NUMLOCK, 0);
    //         libevdev_uinput_write_event (uidev, EV_SYN, SYN_REPORT, 0);
    //         nlstate = !ev.value;
    //         printf("LED event: code=%d, value=%d\n", ev.code, !ev.value);
    //         break;
    //     }
    // }
    
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
                run_non_alrm(uid, "down");
            } else {
                keyc = ev.code;
                // value 2; key held down
                if (ev.value == 2) {
                    if (hold < 1){
                        strcat(pattern,"1");
                        hold++;
                    } else if ((hold-5) % interval == (interval - 1)) {
    
                        run_non_alrm(uid, "hold");
                        hold ++;
                    } else {
                        hold ++;
                    }
                // value 1; key down
                } else if (ev.value == 1) {
                    hold = 0;
                    run_non_alrm(uid, "down");
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
            printf("lost device: waiting for reconnect\n");
            fflush(stdout);
            while (access(argv[2], F_OK) != 0) {
                usleep(500000); //0.5s
            }

            fd = open(argv[2], O_RDONLY);
            if (fd < 0) {
                perror("Failed to reopen device");
                return 1;
            }
            printf("device reconnected\n");
            fflush(stdout);
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
