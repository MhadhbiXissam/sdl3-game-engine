#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "common.h"

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* Function pointers exposed to the game library */

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

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

typedef struct {
    void* camera;
    int typ ; 
    void* parent ; 
    void** childs ; 
    int len ; 
} Entity;

typedef struct {
    int h, w;
    int c;
} Game;



void on_game_created(Game** game) {
    *game = malloc(sizeof(Game));
    Game* self = *game;

    set_game_title("issam super game");

    Vec2i size = get_window_size();
    self->w = size.x;
    self->h = size.y;
    printf("w = %i , h = %i " , self->w , self->h);

    set_window_size((Vec2i){ self->w + 100, self->h + 5 });

    self->c = 0;
}



void on_game_init(void* game){
    printf("Game initialized ...");
    //maximize_window();
}



void on_game_event(void** game){
    // no events yet
}



void on_game_update(void** game) {

    Game* self = *game;   // FIX: correct dereference
    self->w += 1;self->h += 1 ; 
    // set_window_size((Vec2i){ self->w , self->h });


    printf("c = %d", self->c);
}



void on_game_finalize(void** game){
    if (game && *game) {
        free(*game);
        *game = NULL;
    }
    printf("game final");
}
