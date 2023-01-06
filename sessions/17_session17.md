---
layout: post
title: Session 17 Intro to Threads
readtime: true
date: Wed 07 Apr 2021 05:42:12 PM EDT
gh-repo: rhit-csse332/csse332
gh-badge: [star,watch,follow]
---

# Introduction and aim
In this session, we will concretely define threads and learn how to use the `pthreads` library. We
will conduct a speed test to compare multithreading vs multiprocessing. Finally, we will take a look
at what are some common issues with multi-threaded applications so we can set up our dive into
concurrency.

## Logistics
* [Slides](https://rosehulman-my.sharepoint.com/:p:/g/personal/noureddi_rose-hulman_edu/Edaz8g1YS5lEu3yXwdlaud8B2aAj7Gt8pVL9UAt9Gw47zA?e=bPennE)
* [Class notebook section](https://rosehulman-my.sharepoint.com/personal/noureddi_rose-hulman_edu/_layouts/OneNote.aspx?id=%2Fpersonal%2Fnoureddi_rose-hulman_edu%2FDocuments%2FClass%20Notebooks%2FCSSE%20332%20Operating%20Systems&wd=target%28_Content%20Library%2FSession%2017.one%7CDB74E55B-48A2-4446-BEBB-9E38D430D2D9%2F%29)
* [Source code]({{ site.gh_repository_url }}/tree/main/activities/17_threads)
* [Activity submission](https://moodle.rose-hulman.edu/mod/assign/view.php?id=2708154)
* [Clear/Muddy survey](https://moodle.rose-hulman.edu/mod/quiz/view.php?id=2785929)

In-class activity and clear/muddy survey are due on Friday before the start of session 18.

# Outline

|           Topic             |           Time              |
| :-------------------------- | --------------------------- |
| Motivation | 5 minutes |
| Intro to multi-threading and multi-threading vs multi-processing | 10 minutes |
| Intro to `pthreads` | 5 minutes |
| Activity | 10 minutes |
| __Break__ | 2 minutes |
| Concurrency issues | 15 minutes |
| Conclusion and moving forward | 3 minutes |

# Video Recording
It turned out that I forgot to hit record when we started the lecture (what a great way to start
things off Mohammad!!). So Dr Song has kindly provided his videos below:

<iframe src="https://rose-hulman.hosted.panopto.com/Panopto/Pages/Embed.aspx?id=b3bb2e7b-13a8-4066-9b11-ac9f014f12a2&autoplay=false&offerviewer=true&showtitle=true&showbrand=false&start=0&interactivity=all" height="360" width="640" style="border: 1px solid #464646;" allowfullscreen allow="autoplay"></iframe>

<iframe src="https://rose-hulman.hosted.panopto.com/Panopto/Pages/Embed.aspx?id=1ef020c9-87ed-474f-9dda-ac9f015ad342&autoplay=false&offerviewer=true&showtitle=true&showbrand=false&start=0&interactivity=all" height="360" width="640" style="border: 1px solid #464646;" allowfullscreen allow="autoplay"></iframe>

<iframe src="https://rose-hulman.hosted.panopto.com/Panopto/Pages/Embed.aspx?id=ad16b132-24da-4a84-99d0-aca00028e717&autoplay=false&offerviewer=true&showtitle=true&showbrand=false&start=0&interactivity=all" height="360" width="640" style="border: 1px solid #464646;" allowfullscreen allow="autoplay"></iframe>
