cc=gcc
my=my_alloc
sys=sys_alloc
include=include
srcdir=src/
src=$(srcdir)main.c $(srcdir)alloc.c
flags= -O0 -pedantic -pedantic-errors -Wall -Werror -std=gnu11 -I $(include)

all: sys_alloc my_alloc

sys_alloc:
	$(cc) $(flags) -o $(sys) $(srcdir)sys_main.c

my_alloc:
	$(cc) $(flags) -o $(my) $(src)

clean:
	rm -f $(my) $(sys)
