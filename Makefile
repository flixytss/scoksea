# Generated Makefile, Just a template. You can modify me

Var0 = clang

.DEFAULT_GOAL := Link
run:
	
src/lib.c.out: src/lib.c
	@echo -e "[\e[1;32m Compiling file src/lib.c 0.0%... \e[0m]"
	@ccache ${Var0} -c src/lib.c -o build/lib.c.out -fPIC -Iinclude
Link: src/lib.c.out
	@echo -e "[\e[1;32m Linking 100%... \e[0m]"

	@clang build/lib.c.out -o build/scok.so -shared
all:	run	src/lib.c.out	Link
install: all
	sudo mv build/scok.so /usr/lib
	sudo mv ./include/scok.h /usr/include/scok.h
