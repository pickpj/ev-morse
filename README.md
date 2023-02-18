# evtest-morse
I've never written code in C before. :)
The code above is my attempt at making it possible to bind multiple commands to a single key. (single, double, long press, etc.)
Uses sudo privileges because it monitors the keyboard. IDK how else to do it, maybe this is some cybersecurity cardinal sin ¯\\_ (ツ) _/¯ <i>oh well</i>.
# Usage
The number following event varies, check with `sudo evtest /dev/input/event0` and increasing the number gradually
```
gcc -o keym keym.c # compile the code
sudo ./keym /dev/input/event0
```
# Changing timings
The long and short press timing comes from evtest, aka I don't know how to change it.
The expiration timing from no key press is defined in `timer.it_value.tv_sec` and `timer.it_value.tv_usec`
