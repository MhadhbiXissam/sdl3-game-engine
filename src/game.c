// this struct will be shared accros all methods ( engine->game )
typedef struct {
    int score ; 
    
} Game; 

// this methode used to draw opengles stuffs 
void draw(void* e){
    printf("Called draw function ... \n") ; 
}

// this methode called once the engine stared ( window and context created )
void on_engine_started(void* e){
    printf("Called on_engine_started function... \n") ; 
}

// this method called before engine will close (sdl quit )
void on_engine_close(void* e){
    printf("Called on_engine_close function... \n") ; 
}

// this methode called before sdl gl swap window 
void on_engine_swap(void* e){
    Engine* engine = (Engine*)e ; 
    Game* game = (Game*)(engine->game);
    game->score += 1 ; 
    printf("Called on_engine_swap function... \n") ; 
}


// this methode is used to handle sdl3 events 
void eventHandler(void* e){
    Engine* engine = (Engine*)e ; 
    Game* game = (Game*)(engine->game);
    if (engine->event.type){
        printf("recieved event %i\n" , engine->event.type) ; 
        printf("Current score %i " , game->score ); 
    }
}