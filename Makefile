# Makefile
#
# Copyright (C) 2015 - Blake Lowe
#
# This file is part of sox2gba.
#
# sox2gba is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# sox2gba is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with sox2gba. If not, see <http://www.gnu.org/licenses/>.
#

CC=gcc
CFLAGS = -std=c11 -pg -pipe -ggdb -Wall -fdiagnostics-color=auto \
	     -march=native -Og -D_GNU_SOURCE
LDFLAGS = 

default: sox2gba

all: sox2gba

sox2gba: sox2gba.o
	gcc $(CFLAGS) -o sox2gba sox2gba.o $(LDFLAGS)

sox2gba.o: sox2gba.c
	gcc $(CFLAGS) -o sox2gba.o -c sox2gba.c

clean:
	rm -f *~ *.o sox2gba gmon.out
