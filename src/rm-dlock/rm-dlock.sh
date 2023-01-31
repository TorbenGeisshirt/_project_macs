#!/usr/bin/env bash

NAME=rm-dlock
PORT=/dev/ttyUSB1

make all && make $NAME.upload PORT=$PORT && make serialview PORT=$PORT