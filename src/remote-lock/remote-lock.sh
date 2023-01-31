#!/usr/bin/env bash

NAME=remote-lock
PORT=/dev/ttyUSB0

make all && make $NAME.upload PORT=$PORT && make serialview PORT=$PORT