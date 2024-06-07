# Examples  
* <h3>headphone.c</h3>

  * Instant execute function
  * Removed Numlock Check
    * EV_LED check could also be removed  
    * Non-keyboard devices are not able to use ctrl, alt, shift, nl states
  * Device reconnect
    * Ensure device shows up under the same name. See [Extras](../../wiki/Extra) #7
    * For run_hold, memset(pattern, ...) no longer has an effect. Pattern is by default reset after each execution. 
* <h3>evmulti.c</h3>  
  
  * Framework  
    * Only a base, many things not implemented:  
    * Numlock check, uinput device, Reducing permissions  
    * Main logic loop / run_xxx functions, Device reconnect  
  * Needs to be tailored for each use case.

Will probably put something nicer together in python at a later time:  
An early concept gui design made with pygubu designer
![](https://github.com/pickpj/ev-morse/assets/118209356/577ae7e8-f0a6-46f3-844a-5b65d86f2885)
