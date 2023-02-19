# ev-morse
I've never written code in C before (using llm's to their full potential). :) <br>
The code allows for the binding of multiple commands to a single key. (single, double, long press, ... any morse combination)<br>
Uses sudo privileges because it monitors the keyboard. Not sure how else to do it, maybe this is some cybersecurity cardinal sin <br>¯\\_ (ツ) _/¯ <i>oh well</i>.
# Usage
The number following event varies, check with `sudo evtest /dev/input/event0` and increasing the number gradually
```
gcc -o evmorse evmorse.c
sudo ./evmorse /dev/input/event0
```
The first line compiles the code, then the second line runs the program.
# Changing timings
The long and short press timing comes from how events are handled, aka I don't know how to change it. <br>
The expiration timing from no key press is defined in `timer.it_value.tv_sec` and `timer.it_value.tv_usec`
# Changing bindings
The key codes will vary from device to device.<br>
Here is a general example of how the bindings would be set. <br>
```
    if (keyc == 164){
        system("playerctl play-pause");
    } else if (keyc == 165) {
        if (strcmp(pattern, "01") == 0){
            system("playerctl next");
        } else if (strcmp(pattern, "00") == 0){
            system("playerctl position 10+");
        }
    } else if (keyc == 163) {
    }
```
`strcmp()` allows us to compare the pattern with a str. In this case 01 or 00, (short, long) and (short, short) presses respectively. <br>
`keyc` is the variable holding the key code value, to find the keycode use `sudo evtest /dev/input/event0`. (Make sure to set the correct event, look at Usage) <br>
`system()` allows us to run commands as if they were run from the terminal. It is under the assumption that UID is 1000. <br>
# Dependencies
gcc and gcc-libs ? I think, but am not sure.
