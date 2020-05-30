
fatman: main.c
	gcc -Wall main.c -o fatman `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0` 

clean:
	$(RM) fatman

run:
	./fatman
