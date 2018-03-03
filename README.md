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
* `reset`

<h4>Example: </h4>
You run pause on, when display showed 8, than you have been waiting for 4 seconds and run pause off.
On the display you will see `7.` : during first second counter counts to `9`,
second to `9.`, third to `8.` and fourth second `7.`.
Running `reset` you will reset the loop, and it will be repeated from 0.

## Sample 3:

In progress.
