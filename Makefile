all: game

game: game.o conn4.o human.o computer.o rating.o
	gcc -o game game.o human.o computer.o conn4.o rating.o

conn4.o: conn4.c conn4.h
	gcc -std=c99 -c -o conn4.o conn4.c

human.o: human.c human.h player.h conn4.h
	gcc -std=c99 -c -o human.o human.c

computer.o: computer.c computer.h player.h conn4.h
	gcc -std=c99 -c -o computer.o computer.c

rating.o: rating.c rating.h conn4.h
	gcc -std=c99 -c -o rating.o rating.c

game.o: game.c human.h computer.h rating.h conn4.h
	gcc -std=c99 -c -o game.o game.c

clean:
	rm -f *.o game