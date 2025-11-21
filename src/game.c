#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "common.h"


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
    void* camera ; 
} Camera ;

typedef struct {
    int h , w  ; 
    int c ; 
}Game ; 




void on_game_created(void** game_ptr) {
    *game_ptr = malloc(sizeof(Game));
    Game* self = (Game*)(*game_ptr);
    
    set_game_title("issam super game");
    get_window_size(&(self->w), &(self->h));
    set_window_size(self->w + 100, self->h + 5);
    self->c = 0;
}

void on_game_init(void* game){
    printf("Game intialzed ..."); 
    maximize_window();
}

void on_game_event(void** game){

}


void on_game_update(void** game) {

    Game* self = (Game*)game;
    self->c+= 5;
    printf("c = %i" , self->c);


}



void on_game_finalize(void** game ){
    if (game && *game) {
        free(*game);      // free the actual allocation
        *game = NULL;     // avoid dangling pointer
    }
    printf("game final");
}

