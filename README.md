# Testing Branch
Will try to see how to integrate modifier keys without complicating things too much <br>
trying to find a solution in the [AHK source code](https://github.com/AutoHotkey/AutoHotkey/blob/alpha/source/keyboard_mouse.cpp).

Also want to discern between numpad numbers and numpad functions (same keycode.. best way of checking numlock?). <br>
Idea 1:
Send numlock value 1 via [uinput](https://www.kernel.org/doc/html/v4.12/input/uinput.html), <br>
then read led output from /dev/input, <br>
then figure out which state it was in, timed out = 1 : led value 1 = 0. 
```
type 17 (EV_LED), code 0 (LED_NUML), value 1
```


How I feel going through the AHK code [(╯°□°）╯︵ ┻━┻](https://youtu.be/z8hhTn5wAL0)
