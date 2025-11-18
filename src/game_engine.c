#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_opengles2.h> 
#include <stdio.h>

/*............................................................*/
typedef struct {
    float x ; float y ; float z  ; float w ; 
} Vec4f ; 
/*............................................................*/
typedef struct {
    float x ; float y ; float z  ; 
} Vec3f ; 
/*............................................................*/
typedef struct {
    float x ; float y ; float z  ; float w ; 
} Vec4i ; 
/*............................................................*/
typedef struct {
    float x ; float y ; float z  ; 
} Vec3i ; 
/*............................................................*/
/*............................................................*/



/*++++++++++++++++++++++++++++++++++++++++++++++*/
typedef struct Engine Engine; // forward declaration
struct Engine {
    SDL_Window* window;
    SDL_GLContext context;
    Uint32 Windowflags;
    int width, height;
    SDL_Event event; 
    int run ; 
    void (*eventHandler)(Engine*); /* Engine* */
    void (*draw)(Engine*); /* Engine* */
    void (*on_engine_started)(Engine*) ; // call this method when window started 
    void (*on_engine_close)(Engine*) ;  // call this method on engine before it closes 
    void (*on_engine_swap)(Engine*) ;  // call this method before swap
    Vec4f clearcolor ; 
} ;
/*++++++++++++++++++++++++++++++++++++++++++++++*/
bool engine_start(Engine* engine);
/*++++++++++++++++++++++++++++++++++++++++++++++*/
void engine_destroy(Engine* engine);
/*++++++++++++++++++++++++++++++++++++++++++++++*/
void engine_getevent(Engine* engine) ; 
/*++++++++++++++++++++++++++++++++++++++++++++++*/
void engine_swap(Engine* engine) ;
/*++++++++++++++++++++++++++++++++++++++++++++++*/
int engine_run(Engine* engine) ; 
/*++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++++++++++++++++++++++++++++++*/

/*----------------------------------------------------------------------------------------------------------------------------------------------------*/
int main(int argc, char *argv[]){
    Engine engine;
    SDL_zero(engine);
    Vec4f clear_color = {1.0f, 0.0f, 0.0f, 1.0f} ; 
    engine.clearcolor.x = 1.0 ; engine.clearcolor.y = 0.6 ; engine.clearcolor.z = 0.6 ; engine.clearcolor.w = 1.0 ; 
    engine_run(&engine);
    return 0;
}
/*----------------------------------------------------------------------------------------------------------------------------------------------------*/
bool engine_start(Engine* engine){
    SDL_zero(engine->event);
    // Initialize SDL with error checking
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return false;
    }
    // Set GL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    engine->Windowflags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
    engine->width = 600;
    engine->height = 600;
    // Create window with error checking
    engine->window = SDL_CreateWindow("OpenGL ES 2.0 ",
                                      engine->width, engine->height,
                                      engine->Windowflags);
    if (!engine->window) {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }
    // Create GL context
    engine->context = SDL_GL_CreateContext(engine->window);
    if (!engine->context) {
        SDL_Log("SDL_GL_CreateContext failed: %s", SDL_GetError());
        SDL_DestroyWindow(engine->window);
        SDL_Quit();
        return false;
    }
    // Make context current
    if (SDL_GL_MakeCurrent(engine->window, engine->context) < 0) {
        SDL_Log("SDL_GL_MakeCurrent failed: %s", SDL_GetError());
        SDL_GL_DestroyContext(engine->context);
        SDL_DestroyWindow(engine->window);
        SDL_Quit();
        return false;
    }
    // Set initial viewport
    glViewport(0, 0, engine->width, engine->height);
    engine->run = 1 ; 
    return true;
}
//############################################
void engine_destroy(Engine* engine) {
    if (engine->context) {
        SDL_GL_DestroyContext(engine->context);
        engine->context = NULL;
    }
    if (engine->window) {
        SDL_DestroyWindow(engine->window);
        engine->window = NULL;
    }
    SDL_Quit();
}
//############################################
void engine_getevent(Engine* engine) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        // Copy the event to the engine struct if needed elsewhere
        engine->event = event;
        
        if (event.type == SDL_EVENT_QUIT) {
            engine->run = 0;
        }
        if (event.type == SDL_EVENT_WINDOW_RESIZED) {
            int w = event.window.data1;
            int h = event.window.data2;
            glViewport(0, 0, w, h);
        }
    }
}
//############################################
void engine_swap(Engine* engine){
    SDL_GL_SwapWindow(engine->window);
}
//############################################
int engine_run(Engine* engine) {

    if (!engine_start(engine)) {
        return 1;
    }
    if (engine->on_engine_started ) {
        engine->on_engine_started(engine);
    }
    bool has_eventHandler = false ; if (engine->eventHandler) {has_eventHandler = true ; }
    bool has_draw = false ; if (engine->draw) {has_draw = true ; }
    bool has_on_engine_swap= false ; if (engine->on_engine_swap) {has_on_engine_swap = true ; }
    bool has_on_engine_close= false ; if (engine->on_engine_close) {has_on_engine_close = true ; }
    while(engine->run){
        engine_getevent(engine) ; 
        if (has_eventHandler) {
            engine->eventHandler(engine) ; 
        }
        glClearColor(engine->clearcolor.x, engine->clearcolor.y, engine->clearcolor.z, engine->clearcolor.w);
        //printf("engine clear color %f ,%f , %f , %f  ",engine->clearcolor.x, engine->clearcolor.y, engine->clearcolor.z, engine->clearcolor.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (has_draw) {
            engine->draw(engine) ; 
        }
        if (has_on_engine_swap) {
            engine->on_engine_swap(engine) ; 
        }
        engine_swap(engine) ; 
    }
    
    if (has_on_engine_close) {
        engine->on_engine_close(engine) ; 
    }
    engine_destroy(engine);
    return 0;
}
//############################################

//############################################

//############################################

//############################################

//############################################

//############################################

//############################################

//############################################

//############################################

//############################################

//############################################

//############################################

//############################################

//############################################

//############################################