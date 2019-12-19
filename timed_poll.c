/*
 * Sleep for no less than "ms" milliseconds.
 * sleep_ms() is an as yet unimplemented nanosleep() wrapper function.
 */
extern void sleep_ms(uint32_t ms);

/*
 * timed_poll() polls a condition, and if that condition has not occurred,
 * repeats the poll after a specified time delay up to a specified number
 * of delays.
 *
 * timed_poll() takes four arguments:
 * 1) a pointer to a function that takes no arguments and returns a boolean
 * true if the condition being polled has occurred, or false if not.
 * 2) the maximum number of delays between polls before timed_poll returns
 * a boolean false (Note the number of polls taken will be one greater than
 * the number of delays since an initial poll is taken before the first delay),
 * 3) a pointer to a delay function that takes an argument for units of delay,
 * 4) the units of time delay duration between polls in units per delay.
 *
 * timed_poll() returns a boolean:
 * true if the condition occurred
 * false if the maximum delay was reached before the condition occurred.
 *
 * Delay calculation example:
 * timed_poll(&condition_check, 5, &sleep_ms, 10);
 * The maximum delay is:
 * (number of delays) * (units of time delay) * (units of duration) =
 *     5 delays       *        ms/unit        * 10 units/delay      = 50 ms
 */
extern bool timed_poll(bool (*condition_check)(void), uint32_t delays,
    void (*delay_function)(uint32_t), uint32_t delay_duration);

bool timed_poll(bool (*condition_check)(void), uint32_t delays,
    void (*delay_function)(uint32_t), uint32_t delay_duration) {
    for (; delays > 0; delays--) {
        if (condition_check()) /* Poll before delay */
            return true;
        delay_function(delay_duration);
    }
    /*
     * Final poll after the last delay has occurred (or the only
     * poll if this function was called with delays == 0)
     */
    return condition_check();
}

static bool check_status_reg(void) {
    if (GET_STATUS(STATUS_REG) != 0)
        return true;
    return false;
}

bool is_something_running(void) {
    /*
     * Wait up to 1 second (10 delays * 100ms/delay = 1000ms) checking
     * every 100ms.
     */
    return timed_poll(&check_status_reg, 10, &sleep_ms, 100);
}
