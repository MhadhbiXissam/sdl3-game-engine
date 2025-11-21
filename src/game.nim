

type
  ProcGetWindowSize* = proc(w: var ptr cint, h: var ptr cint)  {.noSideEffect.}
var get_window_size* {.exportc:"get_window_size".} : ptr ProcGetWindowSize #Error: attempting to call routine: 'get_window_size'

type
  Game* = ref object
    w*: cint
    h*: cint
    c*: cint

proc on_game_created*(game: var Game) {.exportc,dynlib.}  =
    game = new Game
    game.c = 150
    var x , y  : cint
    if get_window_size != nil:
      get_window_size[]( addr x,  addr y)


    GC_ref(game)



proc on_game_update*(game: var Game) {.exportc,dynlib.}  =
    game.c += 10
    #echo $(game.c)






#   set_game_title("issam super game ")

#   get_window_size(addr g.w, addr g.h)
#   set_window_size(g.w + 100, g.h + 5)


# var get_window_size* : proc (w: var cint, h: var cint) {.exportc,cdecl.} = nil
# var  get_window_size* : proc (w: var cint, h: var cint)  {.exportc,cdecl.}  = nil 
# var set_window_size*: proc (w: cint, h: cint) {.extern,cdecl.}
# get_window_position*: proc (x: ptr cint, y: ptr cint) {.cdecl.} = nil
# set_window_position*: proc (x: cint, y: cint) {.cdecl.} = nil
# set_game_title*: proc (title: cstring) {.cdecl.} = nil
# show_window*: proc () {.cdecl.} = nil
# hide_window*: proc () {.cdecl.} = nil
# maximize_window*: proc () {.cdecl.} = nil
# minimize_window*: proc () {.cdecl.} = nil
# restore_window*: proc () {.cdecl.} = nil