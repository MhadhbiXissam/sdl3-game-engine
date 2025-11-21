import nimpy 
{.emit: """
#include "common.h"
Vec2i (*get_window_size)(void) = NULL;
void  (*set_window_size)(Vec2i) = NULL;
Vec2i (*get_window_position)(void) = NULL;
void  (*set_window_position)(Vec2i) = NULL;
void (*set_game_title)(const char*) = NULL;
void (*show_window)(void) = NULL;
void (*hide_window)(void) = NULL;
void (*maximize_window)(void) = NULL;
void (*minimize_window)(void) = NULL;
void (*restore_window)(void) = NULL;
void (*raise_window)(void) = NULL;
""".}

type
  Vec2i {.importc,nodecl.} = object 
    x*: cint
    y*: cint

  Game* = ref object
    x*: cint
    y*: cint
    c : cint 
# Function pointer types
# type


# Import C functions
proc get_window_size*(): Vec2i {.importc, nodecl.}
proc set_window_size*(v: Vec2i) {.importc, nodecl.}
proc get_window_position*(): Vec2i {.importc, nodecl.}
proc set_window_position*(v: Vec2i) {.importc, nodecl.}
proc set_game_title*(title: cstring) {.importc, nodecl.}
proc show_window*() {.importc, nodecl.}
proc hide_window*() {.importc, nodecl.}
proc maximize_window*() {.importc, nodecl.}
proc minimize_window*() {.importc, nodecl.}
proc restore_window*() {.importc, nodecl.}
proc raise_window*() {.importc, nodecl.}

proc on_game_created*(game: var Game) {.exportc,dynlib.}  =
    game = new Game
    game.c = 150
    let s = get_window_size()
    echo s.x , s.y
    GC_ref(game)



proc on_game_update*(game: var Game) {.exportc,dynlib.}  =
    game.c += 10
    echo "c = " , game.c
    let os = pyImport("os")
    echo "Current dir is: ", os.getcwd().to(string)


proc on_game_event*(game: var Game) {.exportc,dynlib.} =
  # no events yet
  discard



proc on_game_finalize*(game: var Game) {.exportc,dynlib.} =
    GC_ref(game)




