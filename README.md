2048c -- a console implementation of 2048
=========================================

![2048c screenshot](https://cfallin.github.io/2048c/2048c.png)

This is a simple implementation of the popular
[2048](https://gabrielecirulli.github.io/2048/) puzzle game for a text-mode
console, written in C. It should work on any reasonable GNU/Linux or Unix-like
system.

Gameplay is simple: arrow keys to move the tiles up/down/left/right, and Ctrl-C
or 'q' to quit. The 'r' key resets the board. Have fun!

Compiling
---------

Compilation requires [ncurses](https://www.gnu.org/software/ncurses/), a
console text UI library. Most systems should either already have this, or have
a package available for installation. On Debian or Ubuntu, install
`libncurses5-dev`. On Arch, install `ncurses`. On BSD, the library is already
installed.

Assuming you have a C compiler and `make` on your system, just run:

    $ make

Authorship and License
----------------------

`2048c` was written as a quick afternoon hack by (and is copyright (c) 2017 by)
Chris Fallin &lt;cfallin@c1f.net&gt;. It is released under the MIT License.
