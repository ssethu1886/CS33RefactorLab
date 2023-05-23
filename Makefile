# Make x86-64 random byte generators.

# Copyright 2015, 2020, 2021 Paul Eggert

# This program is free software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.

# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

# Optimization level.  Change this -O2 to -Og or -O0 or whatever.
OPTIMIZE = -O2

# The C compiler and its options.
CC = gcc
CFLAGS = $(OPTIMIZE) -g3 -Wall -Wextra -fanalyzer \
  -march=native -mtune=native -mrdrnd

# The archiver command, its options and filename extension.
TAR = tar
TARFLAGS = --gzip --transform 's,^,randall/,'
TAREXT = tgz

default: randall

randall: *.c
	$(CC) $(CFLAGS) *.c -o $@

assignment: randall-assignment.$(TAREXT)
assignment-files = COPYING Makefile randall.c
randall-assignment.$(TAREXT): $(assignment-files)
	$(TAR) $(TARFLAGS) -cf $@ $(assignment-files)

submission-tarball: randall-submission.$(TAREXT)
submission-files = $(assignment-files) \
  notes.txt  options.c options.h output.c output.h \
  rand64-sw.c rand64-sw.h rand64-hw.c rand64-hw.h randall
randall-submission.$(TAREXT): $(submission-files)
	$(TAR) $(TARFLAGS) -cf $@ $(submission-files)

repository-tarball:
	$(TAR) -czf randall-git.tgz .git

.PHONY: default clean assignment submission-tarball repository-tarball

clean:
	rm -f *.o *.$(TAREXT) randall
c1 = then echo "SUCCESS DEFAULT"
c2 = else echo "FAIL DEFAULT"
c3 = then echo "SUCCESS RDRAND STDIO"
c4 = else echo "FAIL RDRAND STDIO"
c5 = then echo "SUCCESS RDRAND WRITE"
c6 = else echo "FAIL RDRAND WRITE"
c7 = then echo "SUCCESS MRAND48 STDIO"
c8 = else echo "FAIL MRAnD48 STDIO"
c9 = then echo "SUCCESS MRAND48 WRITE"
c10 = else echo "FAIL MRAND48 WRITE"
check:
	if [ `./randall 1000000|wc -c` -eq 1000000 ]; $(c1);$(c2); fi
	if [ `./randall -i rdrand -o stdio 1000000| wc -c` -eq 1000000 ]; $(c3);$(c4);fi
	if [ `./randall -i rdrand -o 10000 1000000| wc -c` -eq 1000000 ]; $(c5);$(c6);fi
	if [ `./randall -i mrand48_r -o stdio 1000000| wc -c` -eq 1000000 ]; $(c7);$(c8);fi
	if [ `./randall -i mrand48_r -o 10000 1000000| wc -c` -eq 1000000 ]; $(c9);$(c10);fi
