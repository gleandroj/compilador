#!/bin/bash

gcc-5 -g -I/home/gabriel/Desktop/Projects/compilador/src src/main.c src/*/* -o build/compilador.out && ./build/compilador.out