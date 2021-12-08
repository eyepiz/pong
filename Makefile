# Author: Eduardo Yepiz
# Email: eyepiz@nd.edu
#
# This is the Makefile for Pong

# This is a test

# PP := gcc

# INC := include
# SRC := src
# OBJ := obj
# EXE := exe

# project: project.c
# 	$(PP) $(SRC)/project.c $(OBJ)/gfx2.o -lX11 -o $(EXE)/project

# initialize:
# 	mkdir $(OBJ) $(EXE)

# clean:
# 	rm -f *.o $(OBJ)/* $(EXE)/*

project: project.c
	gcc project.c gfx2.o -lX11 -o project

clean:
	rm project
