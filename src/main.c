

#include "engine.h"

typedef struct {
    int score ; 
    
} GameState; 

void draw(void* e){
    printf("Called draw function ... \n") ; 
}

void on_engine_started(void* e){
    printf("Called on_engine_started function... \n") ; 
}

void on_engine_close(void* e){
    printf("Called on_engine_close function... \n") ; 
}


void on_engine_swap(void* e){
    Engine* engine = (Engine*)e ; 
    GameState* gs = (GameState*)(engine->SceneManager);
    gs->score += 1 ; 
    printf("Called on_engine_swap function... \n") ; 
    
}


void eventHandler(void* e){
    Engine* engine = (Engine*)e ; 
    GameState* gs = (GameState*)(engine->SceneManager);
    if (engine->event.type){
        printf("recieved event %i\n" , engine->event.type) ; 
        printf("Current score %i " , gs->score ); 
    }
}
int main(int argc, char *argv[]){
    Engine e;
    // Initialize all struct members to zero
    engine_zero(&e);
    e.draw = draw ;
    e.on_engine_started = on_engine_started ; 
    e.on_engine_swap = on_engine_swap ;
    e.on_engine_close = on_engine_close ; 
    e.eventHandler = eventHandler ; 
    e.SceneManager  = (GameState*)malloc(sizeof(GameState));
    ((GameState*)e.SceneManager)->score = 0 ; 
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

