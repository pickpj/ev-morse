# Testing Branch
Will try to see how to integrate modifier keys without complicating things too much <br>
trying to find a solution in the [AHK source code](https://github.com/AutoHotkey/AutoHotkey/blob/alpha/source/keyboard_mouse.cpp).

Also want to discern between numpad numbers and numpad functions (same keycode.. best way of checking numlock?). <br>
[Windows](https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes) doesn't differentiate between them <br>
Idea 1:
Send numlock twice ([uinput](https://www.kernel.org/doc/html/v4.12/input/uinput.html) ?), then read led output from /dev/input ?
```
type 17 (EV_LED), code 0 (LED_NUML), value 1
```


How I feel going through the AHK code [(╯°□°）╯︵ ┻━┻](https://youtu.be/z8hhTn5wAL0)
