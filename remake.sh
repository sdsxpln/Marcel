#!/bin/bash

#this script is used to create a Makefile

LFMakeMaker -v +g -Isrc/ -cc='gcc -Wall -DFREEBOX -DUPS -lpthread -lpaho-mqtt3c -std=c99' src/*.c -t=Marcel > Makefile
