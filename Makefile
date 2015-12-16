all: inotify

TIMEOUT ?= 60
INOTIFY_FLAG ?= 'IN_ACCESS'
INOTIFY_STDIN ?= 0
INOTIFY_TARGETS ?= {0};

inotify: inotify.c
	gcc $^ -o $@ -std=gnu99 -DTIMEOUT=$(TIMEOUT) -DINOTIFY_FLAG="$(INOTIFY_FLAG)" -DINOTIFY_STDIN=$(INOTIFY_STDIN) -DINOTIFY_TARGETS=$(INOTIFY_TARGETS)

clean::
	rm -f *.o inotify

.PHONY: all clean install
