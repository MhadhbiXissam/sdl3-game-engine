#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    int id ; 
}GameState ; 




GameState* on_game_started(){
    GameState* state =  (GameState*)malloc(sizeof(GameState));
    printf("game loaded...\n");
    state->id = 120 ;
    return state;
}



void on_game_event(GameState* state , int typ ){
    printf("recieved event id = ...%i.\n",typ); 

}


void on_game_update(GameState* state ){
    state->id += 1 ;
    printf("game updating state.id...%i.\n",state->id); 


}


void on_game_finalize(void* state ){
    free(state) ; 
    printf("game final \n"); 
}

