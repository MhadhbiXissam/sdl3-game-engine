

#include "engine.h"
#include "game.c"

int main(int argc, char *argv[]){
    Engine e;
    // Initialize all struct members to zero
    engine_zero(&e);
    e.draw = draw ;
    e.on_engine_started = on_engine_started ; 
    e.on_engine_swap = on_engine_swap ;
    e.on_engine_close = on_engine_close ; 
    e.eventHandler = eventHandler ; 
    e.Game  = (Game*)malloc(sizeof(Game));
    ((Game*)e.Game)->score = 0 ; 
    if (!engine_init(&e)) {
        return 1;
    }
    if (e.on_engine_started ) {
        e.on_engine_started(&e);
    }
    
    while(e.run){
        getevent(&e) ; 
        if (e.eventHandler) {
            e.eventHandler(&e) ; 
        }
        
        glClear(GL_COLOR_BUFFER_BIT);
        if (e.draw) {
            e.draw(&e) ; 
        }
        if (e.on_engine_swap) {
            e.on_engine_swap(&e) ; 
        }
        engine_swap(&e) ; 
    }
    
    if (e.on_engine_close) {
        e.on_engine_close(&e) ; 
    }
    
    engine_destroy(&e);
    return 0;
}

