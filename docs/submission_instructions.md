---
title: Submission Instructions
layout: page
---

In CSSE 332, you will checkout code via `git` but you will submit your code via Gradescope.

# Getting Code

You will want to install `git` on your Linux OS (be it a virtual machine or real local OS). It's
usually as easy as a package install. For example, on Ubuntu, you can use
```shell
sudo apt install -y git
```

Once it is installed, checkout the corresponding repo for the assignment. For example, to checkout
the repo containing all the material for this class, you can use
```shell
https://github.com/RHIT-CSSE/csse332
```
That should check out a complete copy of the 332 course materials (including slides, sample exams,
etc.) to your local system in a csse332 directory.

As we add new stuff to the repo, you may need to update your local copy. To get the latest, `cd`
into your csse332 directory and then:
```shell
git pull
```

Note: one thing you should definitely *NOT* do is to "fork" the public git repo via github. This is
because if you commit/push your homework solutions, you'll be committing them to a public repo that
other students can find and use to cheat. If you really want to be able to store your homework code
in a git repo, you can create a private repo on github and use that to keep track of your
submissions. Note that you'll still have to submit any solutions to us via Gradescope.

# Submitting Code
You'll submit code via Gradescope. Click on the relevant assignment and upload all the files (you can
drag and drop from your file choose if you like).

## What file should I upload?

Upload all source files. 
* All `.h` or `.c` files, including files you didn't edit (like tests, etc.)
* Also any Makefiles or anything else required to build your code
* Don't upload compiled binary file (`.o` files, executables, etc.)
