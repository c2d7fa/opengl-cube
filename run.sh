#!/bin/sh

gcc -lglfw -lGLEW -lGL -o main main.c && \
  ./main
