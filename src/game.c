#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* Function pointers exposed to the game library */
void (*get_window_size)(int*, int*) = NULL;
void (*set_window_size)(int, int) = NULL;
void (*get_window_position)(int*, int*) = NULL;
void (*set_window_position)(int, int) = NULL;
void (*set_game_title)(const char*) = NULL;
void (*show_window)(void) = NULL;
void (*hide_window)(void) = NULL;
void (*maximize_window)(void) = NULL;
void (*minimize_window)(void) = NULL;
void (*restore_window)(void) = NULL;
void (*raise_window)(void) = NULL;
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
typedef struct {
    int h , w  ; 
}Game ; 




void on_game_created(void* game ){
    game =  (Game*)malloc(sizeof(Game));Game* self = (Game*)game ; 
    set_game_title("issam super game ");
    get_window_size(&(self->w), &(self->h)) ; 
    set_window_size(self->w + 100 , self->h+5) ; 

}

void on_game_init(void* game){

    printf("Game intialzed ..."); 
    maximize_window();
}

void on_game_event(void* game){

}


void on_game_update(void* game) {
    if (!game) return; // prevent crash
    Game* self = (Game*)game;
    self->w += 5;

}


void on_game_finalize(void* game ){
    free(game) ; 
    printf("game final "); 
}

