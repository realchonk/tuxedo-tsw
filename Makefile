CFLAGS ?= -O3 -std=c99
PREFIX ?= /usr/local

all: tuxedo-tsw

help:
	@printf "TUXEDO Touchpad Switcher by Benjamin Stürz\n"
	@printf "Instructions:\n"
	@printf "\tmake all\n"
	@printf "\tsudo make install\n"
	@printf "\tsudo make udev-reload\n"

tuxedo-tsw: tuxedo-tsw.c
	$(CC) -o $@ $< $(CFLAGS)

clean:
	rm -f tuxedo-tsw

install: tuxedo-tsw
	install -Dm755 tuxedo-tsw $(PREFIX)/bin/tuxedo-tsw
	install -Dm644 tuxedo-tsw.1 $(PREFIX)/share/man/man1/tuxedo-tsw.1
	install -Dm644 99-tuxedo-tsw.rules /etc/udev/rules.d/99-tuxedo-tsw.rules

udev-reload:
	udevadm control --reload
	udevadm trigger

.PHONY: all help clean configure install udev-reload
