---
title: Condition Variables Basics
layout: post
date: Fri Oct 15 15:44:03 2021 
readtime: true
gh-repo: rhit-csse332/csse332
gh-badge: [star,watch,follow]
---

# Introduction

In this assignment, we will have a quick practice with condition variables and
mutexes. There are three problems that you must solve using condition variables. 

[Code Folder]({{ site.gh_repository_url }}/tree/main/homeworks/13_condvar_basics)

## In-order

This problem refers to the file `inorder.c`. In this system, there are threads
numbered 1 through 4 and there is a critical section that the threads are trying
to enter. The threads are required to enter the critical section in order (e.g.,
1 enters the critical section first, 2 following that, then 3, and finally 4).

If a thread `n` finishes and thread `n+1` is waiting, then thread `n+1`
immediately executes. __However, if thread `n+1` is not waiting, no threads
should be allowed into the critical section until the correct thread arrives in
order.__ For example, if thread 1 finishes and thread 2 is not yet waiting to
enter the critical section, then no other thread should be allowed in until
thread 2 comes in and finishes. 

## Max

This problem refers to the file `max.c`. In this system, there is a special
critical section in which 3 threads are allowed to be in at once. Once 3 threads
are in the critical section, any further threads must wait until one of the
threads leaves the critical section. 

This problem is very easy to solve using a semaphore initialized to 3. Your job
is to write a solution that solely uses condition variables and mutexes. 


## Producer Consumer Again

This problem refers to the file `prodcons_condvar.c`.In this system, there 2
types of processes - producers and consumers. Producers write integers to the
shared buffer data. Consumers read integers from the share buffer data.
Everybody has to be careful when modifying data and the integer that keeps track
of how many elements are in data. 

When the producers are being slow (i.e., the buffer is empty), the consumers
should be blocked. Alternatively, when the consumers are being slow (i.e., the
buffer is full), the producers should be blocked.

The order in which the items are consumed does not matter, i.e., it does not
need to be the same as the order they were produced. 


## Guidance on how we grade:
Some guidance on how we grade:

* You can get 25% of points if your solution reliably produces correct output with the given data (and small modifications). 0 points if doing this requires weird sleeps or other hacky non-general purpose code.
* You can get 50% points you did above and your solution is plausible and does not do something obviously bad. It's impossible to enumerate every obviously bad thing but here's a big one : having multiple concurrent threads modify a global variable. Basically this is for solutions that basically are OK but miss some edge cases.
* You can get 50% points if you have a fully working solution but you utilize non-standard functions (i.e. stuff other than `pthread_cond_signal/wait`, `pthread_mutex_lock/unlock`)
* A fully correct solution, even if it's not the one we envisioned, is worth 100%


(20 points) In-Order

(20 points) Max

(20 points) Producer and Consumer
