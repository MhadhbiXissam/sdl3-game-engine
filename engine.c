#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdio.h>

typedef struct {
    SDL_Window* window;
    SDL_GLContext context;
    Uint32 Windowflags;
    int width, height;
} Engine;

bool engine_init(Engine* engine);
void engine_destroy(Engine* engine);

int main() {
    Engine e;
    if (!engine_init(&e)) return 1;
    SDL_Delay(10000) ;
    engine_destroy(&e);
    return 0;
}

bool engine_init(Engine* engine) {
    engine->window = NULL;
    engine->context = NULL;

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

    return true;
}

void engine_destroy(Engine* engine) {
    if (engine->context) SDL_GL_DestroyContext(engine->context);  // SDL3 name
    if (engine->window) SDL_DestroyWindow(engine->window);
}
