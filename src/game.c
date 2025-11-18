typedef struct {
    int score ; 
    
} Game; 

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
    Game* game = (Game*)(engine->Game);
    game->score += 1 ; 
    printf("Called on_engine_swap function... \n") ; 
}


void eventHandler(void* e){
    Engine* engine = (Engine*)e ; 
    Game* game = (Game*)(engine->Game);
    if (engine->event.type){
        printf("recieved event %i\n" , engine->event.type) ; 
        printf("Current score %i " , game->score ); 
    }
}