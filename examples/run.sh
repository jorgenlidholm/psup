#!/bin/bash

RUN="mono OFLMachine.exe"
CWD=$(basename $PWD)
PIDFILE=/var/run/$CWD.pid

echo $PIDFILE

case "$1" in
    start)
        echo $$ > $PIDFILE;
        exec 2>&1 $RUN &>/tmp/$CWD.out;
        ;;
    stop)
        if [ -e $PIDFILE ] ; then
            kill $(cat $PIDFILE);
            rm -f $PIDFILE;
        fi
        pkill -9 candump;
        ;;
    *)
        echo "Usage: $SCRIPTNAME {start|stop|status}" >&2
        exit 1
        ;;
esac
exit $?
