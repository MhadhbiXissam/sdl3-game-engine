



clang-engine : 
	gcc -Wall -fPIC -shared src/game.c -o src/libgame.so && gcc -w src/game_engine.c   -I. libs/libSDL3.a  -lm -lGLESv2 -o build/game.out && ./build/game.out 


nimlang-engine : 
	nim c --exceptions:quirky -d:release --mm:orc -d:useMalloc --noMain --app:lib  -o:src/libgame.so src/game.nim
	gcc -w src/game_engine.c   -I. libs/libSDL3.a  -lm -lGLESv2 -o build/game.out && ./build/game.out

build : clang-engine