clox.exe: compiled/chunk.o compiled/compiler.o compiled/debug.o compiled/main.o compiled/memory.o compiled/scanner.o compiled/value.o compiled/vm.o compiled/object.o compiled
	gcc compiled/chunk.o compiled/compiler.o compiled/debug.o compiled/main.o compiled/memory.o compiled/scanner.o compiled/value.o compiled/vm.o compiled/object.o -o clox.exe -fsanitize=address -Wall

compiled/chunk.o: src/chunk.c compiled 
	gcc -c src/chunk.c -o compiled/chunk.o -fsanitize=address -Wall
compiled/compiler.o: src/compiler.c compiled
	gcc -c src/compiler.c -o compiled/compiler.o -fsanitize=address -Wall
compiled/debug.o: src/debug.c compiled
	gcc -c src/debug.c -o compiled/debug.o -fsanitize=address -Wall
compiled/main.o: src/main.c compiled
	gcc -c src/main.c -o compiled/main.o -fsanitize=address -Wall
compiled/memory.o: src/memory.c compiled
	gcc -c src/memory.c -o compiled/memory.o -fsanitize=address -Wall
compiled/scanner.o: src/scanner.c compiled
	gcc -c src/scanner.c -o compiled/scanner.o -fsanitize=address -Wall
compiled/value.o: src/value.c compiled
	gcc -c src/value.c -o compiled/value.o -fsanitize=address -Wall
compiled/vm.o: src/vm.c compiled
	gcc -c src/vm.c -o compiled/vm.o -fsanitize=address -Wall
compiled/object.o: src/object.c compiled
	gcc -c src/object.c -o compiled/object.o -fsanitize=address -Wall

compiled:
	mkdir compiled