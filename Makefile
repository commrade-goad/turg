build: ./src/main.c
	gcc src/main.c -L ./raylib-5.0_linux_amd64/lib -l:libraylib.a -lm -o turg

run: ./turg
	./turg

main: ./src/main.c ./turg
	build
