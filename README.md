# scheduler

I am posting the solution to this challenge myself since it was posted in another answer that "this problem is impossible to solve". That is beyond stupid.

The solution for this challenge is a very well established algorithm in computer science called "Hierarchical Timed Wheels". The 1987 seminal paper by Varghese and Lauck describes it in detail.

It is mathematically proven there that both the schedule() and check() routines are O(1) in average.

http://www.cs.columbia.edu/~nahum/w6998/papers/sosp87-timing-wheels.pdf

There are several implementations of the algorithm.  libuv itself implements it

https://github.com/libuv/libuv/blob/v1.x/test/benchmark-million-timers.c

Since then I deleted my account for good on StackOverflow. The place is toxic.

Enjoy!
