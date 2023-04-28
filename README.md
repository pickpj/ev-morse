# Testing Branch
Will try to see how to integrate modifier keys without complicating things too much <br>
trying to find a solution in the [AHK source code](https://github.com/AutoHotkey/AutoHotkey/blob/alpha/source/keyboard_mouse.cpp).

ToDo: <br>
Decide action in edge cases / how to interact with pattern when multiple keys are pressed <br>
Whether to use Struct, array, etc.<br>
Have hold / pattern values with each key in a tuple? <[keycode, hold, pattern?], ...[]> <br>
Completely rewrite logic :( <br>
Can't rely on itimer anymore? multithread?

lazy solution for now, bool for each modifier key. ✔️
- Cant have bindings on shift, alt, ctrl (no longer show in key code outputs)
- Not sure if it is the most efficient (Every key event has to perform 6 more checks)
  - if (ev.code == 97 | ev.code == 29 | ev.code == 54 | ev.code == 42 | ev.code == 100 | ev.code == 56)



~~Also want to discern between numpad numbers and numpad functions.~~ <br>
Idea 1  (implemented ✔️):<br>
Send numlock via [uinput](https://www.kernel.org/doc/html/v4.12/input/uinput.html), <br>
then reads led output from /dev/input to determine nlstate, `type 17 (EV_LED), code 0 (LED_NUML), value 1`

Comments: <br>
Pause before writing to uinput for 1 second. <br>
Time between writes is fast enough that lock keys status in KDE Plasma does not respond. <br>
Code starting to feel bulky without modifiers being added yet.

<br>


How I feel going through the AHK code [(╯°□°）╯︵ ┻━┻](https://youtu.be/z8hhTn5wAL0)
