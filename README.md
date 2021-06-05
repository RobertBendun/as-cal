# as-cal

Simple calendar utility for viewing events in monthly / weekly / yearly context
with goal of beeing part of minimalist task management system.

## Example
```
$ cat example
01.06 10:00 Task nr 1
08.06 10:00 Task nr 2
08.06 11:00 Task nr 3
10.06 20:00 Task nr 4
02.07 10:15 Task nr 5
$ cat example | ./as-cal
      June 2021
Mo Tu We Th Fr Sa Su
    1  2  3  4  5  6
 7  8  9 10 11 12 13
14 15 16 17 18 19 20
21 22 23 24 25 26 27
28 29 30
08.06 => 10:00 Task nr 2
         11:00 Task nr 3
10.06 => 20:00 Task nr 4

      July 2021
Mo Tu We Th Fr Sa Su
          1  2  3  4
 5  6  7  8  9 10 11
12 13 14 15 16 17 18
19 20 21 22 23 24 25
26 27 28 29 30 31
02.07 => 10:15 Task nr 5
```

## Dependencies
- C++20
- [Boost.Date\_Time](https://www.boost.org/doc/libs/1_62_0/doc/html/date_time.html)
