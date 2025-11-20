#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
typedef struct Engine Engine; // forward declaration

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void (*get_window_size)(int*, int*) = NULL;
void (*set_game_title)(const char* ) = NULL ; 
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
typedef struct {
    int id ; 
}Game ; 




void on_game_created(void* game ){
    game =  (Game*)malloc(sizeof(Game));Game* self = (Game*)game ; 
    printf("game created...\n");
    self->id = 120 ;
}

void on_game_init(void* game){
    set_game_title("issam super game ");
    printf("Game intialzed ..."); 
}

void on_game_event(void* game){

}


void on_game_update(void* game ){
    Game* self = (Game*)game ; 
    self->id += 1 ;
    printf("game updating game.id...%i.\n",self->id); 
    int w , h ; 
    get_window_size(&w,&h);
    printf("the window size : w = ...%i.\n",w); 
}


void on_game_finalize(void* game ){
    free(game) ; 
    printf("game final "); 
}

