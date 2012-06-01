What is this?
=============

My son's science fair project which I've already spent way to much time on.
It's our first hardware hacking, so be gentle. I'm not much of a C hacker and
my Makefile is pretty disgusting. But it works. I am a professional hacker
though, so I do feel a little embarrassed by it. Hopefully it can help someone
else get off to a good start.

The idea is to drive an HT1632, but we are running of time and it's not
cooperating. Not sure if it's a bad unit or what. We may end up just doing the
prototypical blinking LED, but we still are holding out.

Dependencies
============

You need https://github.com/mjoldfield/Arduino-Makefile.git. Makefile is
pointing to $HOME/arc/Arduino-Makefile. Edit that if you put it somewhere else.

Install Arduino
===============

This installs version 1.0.1, which includes the toolchain. Very nice. This will
save you time, just do it.

  $ bin/install-arduino

Configure
=========

Things are setup for an atmega2560. Edit Makefile for a different type of board.

Build
=====

  $ make

Upload
======

  $ make upload

Monitor
=======

I've changed monitor so that it reattaches to the screen session after the
first time. You won't be able to upload while it's running though, so there is
a make recipe to kill it before upload. Should work automatically, but knowing
is half the battle.

  $ make monitor

LICENSE
=======

MIT -- Blah blah. Do what you want.

Arduino-Makefile is LGPL.
