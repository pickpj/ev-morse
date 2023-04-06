# ev-morse
The code allows for the binding of multiple commands to a single key. (single, double, long press, ... any morse combination) (also can now bind commands to repeat when being held down)<br>
Uses root privileges because it reads output from /dev/input. <br>
¯\\_ (ツ) _/¯ <i>oh well</i>, So be sure to read the code, the non-personalized sections of code are ~100 lines.

# Usage
The number following event varies, check with `sudo evtest /dev/input/event0` and increasing the number gradually
```
gcc -o evmorse evmorse.c
sudo ./evmorse /dev/input/event0
```
The first line compiles the code, then the second line runs the program.

# Changing bindings
The key codes will vary from device to device.<br>
&emsp; *IMPORTANT!* &emsp; Keycodes differ between X11 and /dev &ensp; (Ex. For me numpad minus is 82 in xev, but 74 in evtest)<br>

## run_hold and run_pattern
The code is separated into two functions.
run_hold - For long key presses we can execute a command as the key is held down (think of a fast-forward function on a remote) <br>
run_pattern - This is for executing commands based on a morse pattern (01 = short | long; 110 = long | long | short; etc.) <br>
For a key that is defined in both functions we may not want to run the pattern after the hold function, thus we must reset the value of the pattern variable with <br>
`memset(pattern, 0, sizeof(pattern));` <br>
Here is a general example of how the bindings would be set. <br>
```
static void run_hold() {
    int len = strlen(pattern);
    // vvv Your code goes here vvv
    switch (keyc) {
        case 163:
            system("playerctl position 5+");
            memset(pattern, 0, sizeof(pattern));
            break;
        case 165:
            system("playerctl position 5-");
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
        case 164:
            system("playerctl play-pause");
            break;
        case 163:
            for(int i = 0; i < len; i++) {
                system("playerctl next");
            } 
            break;
        case 165:
            for(int i = 0; i < len; i++) {
                system("playerctl previous");
            } 
            break;
        }
        // ^^^ Your code goes here ^^^
        memset(pattern, 0, sizeof(pattern));
    }
}
```
`keyc` is the variable holding the key code value, to find the keycode use `sudo evtest /dev/input/event0`. (Make sure to set the correct event, look at Usage) <br>
`system()` allows us to run commands as if they were run from the terminal. It is under the assumption that UID is 1000. <br>
In the above example a short key press of the key with code 163 would result in "playerctl next" executing (next song). <br>
However, when holding the key "playerctl position 5+" in the run_hold function would execute instead (fast forward). <br>
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


# Dependencies
gcc and gcc-libs ? I think, but am not sure.

# Quirks
- After opening the /dev/input the UID is changed to 1000 (default non root user). This could be changed if we wanted to run commands as a different user.
- The low level manner in which the keyboard is capture means that functions work when a key is disabled with xmodmap

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
