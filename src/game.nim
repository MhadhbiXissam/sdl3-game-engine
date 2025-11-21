var
  get_window_size* {.exportc,dynlib.}: proc (w: var cint, h: var cint) : void  = nil 
  set_window_size*: proc (w: cint, h: cint) {.cdecl.} = nil
  get_window_position*: proc (x: ptr cint, y: ptr cint) {.cdecl.} = nil
  set_window_position*: proc (x: cint, y: cint) {.cdecl.} = nil
  set_game_title*: proc (title: cstring) {.cdecl.} = nil
  show_window*: proc () {.cdecl.} = nil
  hide_window*: proc () {.cdecl.} = nil
  maximize_window*: proc () {.cdecl.} = nil
  minimize_window*: proc () {.cdecl.} = nil
  restore_window*: proc () {.cdecl.} = nil
  raise_window*: proc () {.cdecl.} = nil



type
  Game* = ref object
    w*: cint
    h*: cint
    c*: cint

proc on_game_created*(game: var Game) {.exportc,dynlib.}  =
    game = new Game
    game.c = 150
    GC_ref(game)
    # game = cast[ptr Game](alloc(sizeof(Game)))
    # # game[].c = 120


proc on_game_update*(game: var Game) {.exportc,dynlib.}  =
    game.c += 10
    echo $(game.c)






#   set_game_title("issam super game ")

#   get_window_size(addr g.w, addr g.h)
#   set_window_size(g.w + 100, g.h + 5)
