# inotifyd

Monitors flag files for reads (or writes, if you want).  Prints a message when read.

After the first flag read, successive reads are accumulated and posted every 60 seconds.

The code is a little bit over-engineered to handle monitoring multiple flag files and multiple conditions (e.g. for monitoring when the flag file is modified, to synchronize game round clocks).

## Usage

These commands are intended to be run on the machine being defended.

The file to be monitored is hard-coded to be `/home/flags/$USER`.

```shell
$ SERVICE=rxc
$ INOTIFY="/home/$SERVICE/inotify"
$ cp inotify "$INOTIFY"
$ sudo -u "$SERVICE" "$INOTIFY"
```

If the file `/home/flags/rxc` is read, you should see some output that looks like:

```
inotify pwned 2015-12-15 20:01:32.077485 IN_ACCESS "/home/flags/rxc"
```

The format of the data is intended for [Slack integration](https://github.com/samuraictf/slack).  To enable Slack integration, just pipe the output of the last command to `/home/ctf/slack`.

The alternate modes can be activated by rebuilding the project.  For example, we can monitor for reads *or* writes by changing the flags.

```shell
$ make clean all INOTIFY_FLAG='IN_ACCESS|IN_MODIFY'
```
