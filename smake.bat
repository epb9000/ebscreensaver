@echo off
gcc -c scrn.c
gcc scrn.o scrn.cof -l scrnsave -l scrnsavw -l gdi32 -l winmm -l zlib -lmsimg32 -ldumbd -o a.scr -mwindows
strip a.scr
