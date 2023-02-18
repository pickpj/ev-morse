# evtest-morse
I've never written code in C before. :)
The code above is my attempt at making it possible to bind multiple commands to a single key. (single, double, long press, etc.)
Uses sudo privileges because it monitors the keyboard. IDK how else to do it, maybe this is some cybersecurity cardinal sin ¯\\_ (ツ) _/¯ <i>oh well</i>.
# Usage
gcc -o keym keym.c # compile
sudo keym /dev/input/event0 # The number following event varies, check with "sudo evtest /dev/input/event0" and increasing the number gradually
