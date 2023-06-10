# ev-morse
The code allows for the binding of multiple commands to a single key. (single, double, long press, ... any morse combination) <br>
Additionally there is a secondary function (run_hold) to repeat commands when being held down longer. <br>
Uses root privileges because it reads output from /dev/input. <br>
So be sure to read the code.

# Usage
The number following event varies, check which device you would like to interface with, with `sudo evtest`
```
gcc $(pkg-config --libs --cflags libevdev) -o evmorse evmorse.c
sudo ./evmorse $(id -u) /dev/input/event0
```
The first line compiles the code, then the second line runs the program on event##, using the uid from id -u. <br>
Try typing! Once you see the output you can get a better understanding of when commands execute.

# Changing bindings
The key codes may vary from device to device.<br>
&emsp; *IMPORTANT!* &emsp; Keycodes differ between X11 and /dev &ensp; (Ex. For me numpad minus is 82 in xev, but 74 in evtest)<br>
Variables {nlstate; shiftstate; ctrlstate; altstate} can be used to have different behavior when modifiers are pressed or the numlock is off/on.

## run_hold and run_pattern
The code is separated into two functions. <br>
run_hold - For long key presses we can execute a command as the key is held down (think of a fast-forward function on a remote) <br>
run_pattern - This is for executing commands based on a morse pattern (01 = short | long; 110 = long | long | short; etc.) <br>
For a key that is defined in both functions we may not want to run the pattern after the hold function, thus we must reset the value of the pattern variable with <br>
`memset(pattern, 0, sizeof(pattern));` <br>
Here is a general example of how bindings might be set. <br>
```diff
static void run_hold() {
    int len = strlen(pattern);
+    // vvv Your code goes here vvv
    switch (keyc) {
        case 163:
!            system("playerctl position 5+");
!            memset(pattern, 0, sizeof(pattern));
            break;
        case 165:
            system("playerctl position 5-");
            memset(pattern, 0, sizeof(pattern));
            break;
    }
+    // ^^^ Your code goes here ^^^
}

static void run_pattern() {
    int len = strlen(pattern);
    if (len >= 1) {
+        // vvv Your code goes here vvv
        switch (keyc) {
        case 164:
            system("playerctl play-pause");
            break;
        case 163:
!            for(int i = 0; i < len; i++) {
!                system("playerctl next");
!            } 
            break;
        case 165:
            for(int i = 0; i < len; i++) {
                system("playerctl previous");
            } 
            break;
        }
+        // ^^^ Your code goes here ^^^
    memset(pattern, 0, sizeof(pattern));
    }
}
```
`keyc` is the variable holding the key code value, to find the keycode use `sudo evtest /dev/input/event0`. (Make sure to set the correct event, look at Usage) <br>
`system()` allows us to run commands as if they were run from the terminal. It is ran under the bus of the UID returned by (id -u). <br>
In the above example a short key press of the key with code 163 would result in "playerctl next" executing &emsp;(next song). <br>
However, when holding the key, "playerctl position 5+" in the run_hold function would execute instead &emsp;(fast forward). <br>
Additionally, (after fast forwarding) because we used `memset` the pattern variable is empty and fails the `if(len >=1)` check. Preventing the run_pattern function from executing any commands. <br>

Another important operator is `strcmp()`. <br>
This allows us to compare the pattern with a str to separate by the morse pattern. Ex:

```
if (strcmp(pattern, "00") == 0) {            // double short press
    system("");
} else if (strcmp(pattern, "1") == 0) {      // one long press
    system("");
}
```


# Changing timings
The expiration timing from no key press is defined in `timer.it_value.tv_sec` and `timer.it_value.tv_usec` <br>
The interval value controls the interval on which the run_hold function is ran.<br>
A key press would have a value of 1 after around 250ms. If we keep holding for around 300ms (30ms x 10; default interval is 10) the run_hold function is executed. <br>
Referring back to the earlier example, holding for 550ms would fast forward and repeat every 300ms; whereas, any key presses released prior to 550ms would go to the next song (after the expiration timing; default 300ms). <br>

# Diagram
If my explanations suck here is a picture that hopefully sucks less. <br>
![key](https://user-images.githubusercontent.com/118209356/230539020-6f826b2f-e3b0-4eb2-877c-d4fb2ba25c7e.png)


# Quirks / Extra
- The low level manner in which the keyboard is capture means that functions work when a key is disabled with xmodmap
- Can be used with more than just the keyboard, can interact with anything that outputs to events (may need to modify some code) 
    - Ex. In evtest the laptop lid switch outputs show up  as  
    ```
    Event: time 1681146031.388762, type 5 (EV_SW), code 0 (SW_LID), value 1
    Event: time 1681146031.388762, -------------- SYN_REPORT ------------
    Event: time 1681146032.121323, type 5 (EV_SW), code 0 (SW_LID), value 0
    Event: time 1681146032.121323, -------------- SYN_REPORT ------------
    ```
    - If we changed `if(ev.type == EV_KEY)` to EV_SW we are able to execute commands under code 0. ([more info on event types](https://www.kernel.org/doc/html/v4.17/input/event-codes.html#event-types))
    - Also would need to remove the Numlock libevdev section (the lines with libevdev_uinput_write_event and the while loop), and check if there are no interferences with the keycodes reserved for modifiers keys (29,42,54,56,97,100).
- Depending on what you are doing, it may help to append an & at the end of the `system()` command. &emsp; [more info](https://stackoverflow.com/questions/6962156/is-there-a-way-to-not-wait-for-a-system-command-to-finish-in-c) However, this may introduce race conditions, with commands competing over the same resources at the same time.
- More on interactions with [fn key](https://askubuntu.com/questions/270416/how-do-fn-keys-work)
- If .Xmodmap isn't enough for disabling a key (some keys are still picked up by browsers; eg. F7) [this wiki](https://wiki.archlinux.org/title/Map_scancodes_to_keycodes) covers how to remap it to an unused key, maybe f13-f24.
- Still works when you switch tty's, probably a con, depending on your use case.

# More binding examples
My audio control setup:
```
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
// . ^^ run_hold ^^
// .   
// . vv run_pattern vv
        
        switch (keyc) {
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
                system("playerctl -p spotify volume 0.7 || playerctl -p youtube-music volume 15 || playerctl --player=cmus,%%any volume 0.7");
            } else {
                system("playerctl -p spotify volume 1 || playerctl -p youtube-music volume 30 || playerctl --player=cmus,%%any volume 1");
            }
            break;
        }
```
Send key presses without using xdotool. <br> This example presses grave key while shift is held down for a tilde ~<br>
\* ensure that the device is not destroyed (by libevdev_uinput_destroy (uidev);) and that the key codes to be used are defined (with libevdev_enable_event_code (dev, EV_KEY, KEY_~~~, NULL);)
```
                    libevdev_uinput_write_event (uidev, EV_KEY, KEY_LEFTSHIFT, 1);
                    libevdev_uinput_write_event (uidev, EV_SYN, SYN_REPORT, 0);
                    libevdev_uinput_write_event (uidev, EV_KEY, KEY_GRAVE, 1);
                    libevdev_uinput_write_event (uidev, EV_SYN, SYN_REPORT, 0);
                    libevdev_uinput_write_event (uidev, EV_KEY, KEY_GRAVE, 0);
                    libevdev_uinput_write_event (uidev, EV_SYN, SYN_REPORT, 0);
                    libevdev_uinput_write_event (uidev, EV_KEY, KEY_LEFTSHIFT, 0);
                    libevdev_uinput_write_event (uidev, EV_SYN, SYN_REPORT, 0);
```
Send left click to position (w/ xdotool): <br>
\* Uses regex to parse output of xdotool getmouselocation, which is why it's so bulky *
```
FILE* fp;
char mpos[50];
regex_t regex;
int reti;
const char *mregex = "x:([0-9]+) y:([0-9]+)";
size_t nmatch = 3;
regmatch_t pmatch[3];


// ^^ Place above the function ^^

        case 65:
            fp = popen("xdotool getmouselocation", "r");
            fgets(mpos, sizeof(mpos)-1, fp);
            reti = regcomp(&regex, mregex, REG_EXTENDED);
            reti = regexec(&regex, mpos, nmatch, pmatch, 0);
            pclose(fp);
            system("xdotool mousemove 1900 1050 && xdotool click 1");
            sprintf(pattern, "xdotool mousemove %.*s %.*s", (int)(pmatch[1].rm_eo - pmatch[1].rm_so), mpos + pmatch[1].rm_so, (int)(pmatch[2].rm_eo - pmatch[2].rm_so), mpos + pmatch[2].rm_so);
            system(pattern);
            regfree(&regex);
            break;
```