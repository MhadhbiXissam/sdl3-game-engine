#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_opengles2.h> 
#include <stdio.h>


typedef struct {
    SDL_Window* window;
    SDL_GLContext context;
    Uint32 Windowflags;
    int width, height;
    SDL_Event event; 
    int run ; 
    void (*eventHandler)(void*); /* Engine* */
    void (*draw)(void*); /* Engine* */
    void (*on_engine_started)(void*) ; // call this method when window started 
    void (*on_engine_close)(void*) ;  // call this method on engine before it closes 
    void (*on_engine_swap)(void*) ;  // call this method before swap
    void* game ; // DATA CAN BE USED FOR THE GAME AND SET TO BE GLOBAL 
    float clearColor_r ; float clearColor_g ; float clearColor_b ; 
} Engine;





bool engine_start(Engine* engine);
void engine_destroy(Engine* engine);
void getevent(Engine* engine) ; 
void engine_swap(Engine* engine) ; 
int engine_run(Engine* engine) ; 


