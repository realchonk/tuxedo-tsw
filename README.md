# tuxedo-tsw
Unofficial TUXEDO Touchpad Switcher.

## Description/Why
The official driver didn't work for me since it was broken and required a desktop environement, since I use [dwm](dwm.suckless.org) as my window manager.
Also I didn't like that the official driver has about 10 times the needed SLoC.
You can see tuxedo-tsw as a "suckless" alternative (that actually works).

## Dependencies
- C compiler and make
- the HIDRAW Linux kernel driver (CONFIG_HIDRAW=y)
- a supported device (see man page)

## Installation
See
<code>
make help
</code>

## Unimplemented Features
- Double click on LED

## Credits
Some code was copied from the [official driver](https://github.com/tuxedocomputers/tuxedo-touchpad-switch).
