
fatman: main.c
	gcc -Wall main.c -o fatman `pkg-config --cflags --libs gtk+-3.0`

clean:
	$(RM) fatman

run:
	./fatman
