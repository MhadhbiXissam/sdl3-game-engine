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
    void (*on_engine_started)(void*) ; // call this methode when window started 
    void (*on_engine_close)(void*) ;  // call this method on engine before it closes 
    void (*on_engine_swap)(void*) ;  // call this method on engine before it closes 
    void* SceneManager ; // DATA CAN BE USED FOR THE GAME AND SET TO BE GLOBAL 
    float clearColor_r ; float clearColor_g ; float clearColor_b ; 
} Engine;

bool engine_init(Engine* engine);
void engine_destroy(Engine* engine);
void getevent(Engine* engine) ; 
void engine_swap(Engine* engine) ; 
// void HandleEvent(Engine* engine) ; 
int main() {
    Engine e;
    if (!engine_init(&e)) return 1;
    if (e.on_engine_started != NULL) {e.on_engine_started(&e) ; }
    while(e.run){
        getevent(&e) ; 
        if (e.eventHandler != NULL) {
            e.eventHandler(&e) ; 
        }
        glClear(GL_COLOR_BUFFER_BIT);
        if (e.draw != NULL) {

            e.draw(&e) ; 
        }
        if (e.on_engine_swap != NULL) {
            e.on_engine_swap(&e) ; 
        }
        engine_swap(&e) ; 
    }
    if (e.on_engine_close != NULL) {
        e.on_engine_close(&e) ; 
    }
    engine_destroy(&e);
    return 0;
}








bool engine_init(Engine* engine) {
    engine->window = NULL;
    engine->context = NULL;
    engine->run = 0 ; 
    engine->clearColor_r = 1.0 ; engine->clearColor_g = 0.0 ; engine->clearColor_b = 0.0 ; 
    SDL_zero(engine->event);  /* SDL will copy this entire struct! Initialize to keep memory checkers happy. */
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    engine->Windowflags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
    engine->width = 600;
    engine->height = 600;

    engine->window = SDL_CreateWindow("OpenGL ES 2.0 Triangle",
                                      engine->width, engine->height,
                                      engine->Windowflags);

    if (!engine->window) return false;

    engine->context = SDL_GL_CreateContext(engine->window);
    if (!engine->context) return false;

    if (SDL_GL_MakeCurrent(engine->window, engine->context) < 0) return false;
    engine->run = 1 ; 
    return true;
}

void engine_destroy(Engine* engine) {
    if (engine->context) SDL_GL_DestroyContext(engine->context);  // SDL3 name
    if (engine->window) SDL_DestroyWindow(engine->window);
    SDL_Quit();
}

void getevent(Engine* engine) {
    SDL_zero(engine->event); 
    while (SDL_PollEvent(&(engine->event ))) {
        if (engine->event.type == SDL_EVENT_QUIT) {
            engine->run = 0;
        }
        if (engine->event.type == SDL_EVENT_WINDOW_RESIZED) {
            int w = engine->event.window.data1;
            int h = engine->event.window.data2;
            glViewport(0, 0, w, h);
        }
    }
}

void engine_swap(Engine* engine){
    glClearColor(engine->clearColor_r,engine->clearColor_g,engine->clearColor_b,1);
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(engine->window);
}