# Examples  
* <h3>headphone.c</h3>

  * Instant execute function
  * Removed Numlock Check
    * EV_LED check could also be removed  
    * Non-keyboard devices are not able to use ctrl, alt, shift, nl states
  * Device reconnect
    * Ensure device shows up under the same name. See [Extras](../../wiki/Extra) #7
    * For run_hold, memset(pattern, ...) no longer has an effect. Pattern is by default reset after each execution. 
