#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <libevdev/libevdev.h>
#include <libevdev/libevdev-uinput.h>
#include <linux/input-event-codes.h>
#include <asm-generic/errno-base.h>

#define ANSI_RED     "\x1b[31m"
#define ANSI_RESET   "\x1b[0m"

// main variables
char pattern[100] = "";
int keyc = -1;
int hold = 0;
int skip = 0;

int rc;
struct libevdev_uinput *uidev;
struct libevdev *dev[2];
struct input_event ev;



int main(int argc, char **argv)
{
    // Check for proper input of command
    struct input_event ev;
    if (argc < 3) {
        printf(ANSI_RED "Usage: " ANSI_RESET "sudo -E %s $(id -u) <input devices>\n", argv[0]);
        return 1;
    }
    int fd[argc-2];
    int missingdev[argc-2];
    memset(missingdev, 0,(argc-2)*sizeof(int));
    
    for(int i=0;i<argc-2;i++)
    {
        fd[i] = open(argv[i+2], O_RDONLY);
        if (fd[i] < 0)
        {
            printf(ANSI_RED "Error opening file" ANSI_RESET " %s!\n",argv[i+2]);
            exit(0);
        }
        rc = libevdev_new_from_fd(fd[i], &dev[i]);
        if (rc < 0) {
            perror("Failed to initialize libevdev device from fd");
        }
        printf("Device: %s\n", libevdev_get_name(dev[i]));
        fflush(stdout);
    }
    
    while (1) {
        for(int i=0;i<argc-2;i++) {
            if ((libevdev_has_event_pending(dev[i])) > 0) {
                libevdev_next_event(dev[i], LIBEVDEV_READ_FLAG_NORMAL,  &ev);
                if (ev.type != EV_SYN && ev.type != EV_MSC) {
                    printf("%s :: %i :: %i\n", libevdev_event_type_get_name(ev.type), ev.code, ev.value);
                    fflush(stdout);
                }
            }
        }
    }

    hold = 0;
    
    
    for(int i=0;i<argc-2;i++) {
        libevdev_free(dev[i]);
        close(fd[i]);
    }
}