# scheduler

I am posting the solution to this challenge myself since it was posted in another answer that "this problem is impossible to solve". That is beyond stupid.

The solution for this challenge is a very well established algorithm in computer science called "Hierarchical Timed Wheels". The 1987 seminal paper by Varghese and Lauck describes it in detail.

http://www.cs.columbia.edu/~nahum/w6998/papers/sosp87-timing-wheels.pdf

It is mathematically proven there that both the schedule() and check() routines are O(1) in average.

There are several implementations of the algorithm on Github:

https://www.snellman.net/blog/archive/2016-07-27-ratas-hierarchical-timer-wheel/

libuv itself implements it

https://github.com/libuv/libuv/blob/v1.x/test/benchmark-million-timers.c

Here is a benchmark for several implementations of variations of this algorithm

https://www.opensourceagenda.com/projects/timer-benchmarks

Since then I deleted my account for good on StackOverflow. The place is toxic and full of dimwits.

Enjoy!
