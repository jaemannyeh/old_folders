#!/bin/bash
#

prog=yawrz_server

start {
    echo -n $"Starting $prog: "    
}

stop {
    echo -n $"Shutting down $prog: "
}

case "$1" in
    start)
        start
        ;;
    stop)
        stop
        ;;
    status)
        status $prog
        ;;
    restart)
        stop
        start
        ;;
    *)
        echo $"Usage: $0 {start|stop|status|restart}"
        exit 2
        ;;
esac