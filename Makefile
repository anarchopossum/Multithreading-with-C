all:
	gcc rogue.c dungeon_x86_64.o -o rogue -lrt -pthread
	gcc barbarian.c dungeon_x86_64.o -o barbarian -lrt -pthread
	gcc wizard.c dungeon_x86_64.o -o wizard -lrt -pthread
	gcc game.c dungeon_x86_64.o -o game -lrt -pthread
