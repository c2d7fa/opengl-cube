#!/bin/sh

gcc -lm -lglfw -lGLEW -lGL -o main main.c && \
  ./main
