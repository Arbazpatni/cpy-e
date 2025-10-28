#!/bin/bash
#run.sh
function thread() {
    until /app/Cli "10000" #absolute path is very important otherwise error
    do
        echo "Process Crasher with exit code $?, Respawning ..."
        sleep 1;
    done
}

thread   # start worker
