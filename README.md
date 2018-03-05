# Arduino samples
Here you can find samples for Arduino Uno.

Documentation is provided in the `documentation.pdf`

## Sample 1:
You have 8 digit LED display. Digits have such a view:

```
 ___________________________
/                           \
|      a         Also there |
|     ___        ground pin |
|  f |   |  b    should be. |
|    |___|                  |
|  e | g |  c               |
|    |___|.                 |
|          \                |
|      d    dp (point digit)|
\___________________________/     
```

Using Arduino IDE, load `SAMPLE1.ino` on the board.
So now you can see algorithm in work.
It counts from 0 to 9, then digit point lights on and it goes down from 9 to 0.
Reaching 0 loop repeats.

## Sample 2:
Working idea is similar to `SAMPLE1.ino`.
But we can use serial monitor, which supports commands like :

* `pause on` - pauses (freezes) the LED display, but the counter continues counting.
* `pause off` - keeps going (unfreeze) the LED display.
It unfreezes with a current counter value.
* `delay %time` - set time for showing one numeral
* `reset`
* `help` - show help dialog

<h4>Example: </h4>
You run pause on, when display showed 8, than you have been waiting for 4 seconds and run pause off.
On the display you will see `7.` : during first second counter counts to `9`,
second to `9.`, third to `8.` and fourth second `7.`.
Running `reset` you will reset the loop, and it will be repeated from 0.
By default time = 1000 (1s), so if you will run `delay 500`, it will start working twice faster.

<h4>Add your own command:</h4>

* add new enum value in `enum LED_CMD` (eg. `COMMAND`)
* add `#define COMMAND_STR "command\0"` - max 8 digits
* add `#define VALUE_STR "val\0"` - max 4 digits
* add declaration and implementation for `bool check_COMMAND(Command \*)`
* add `if(check_COMMAND(cmd)) return;` to `initCommand`
* add `case COMMAND:` to `switch(cmd->led_cmd)` in `loop()`


## Sample 3:

In progress.
