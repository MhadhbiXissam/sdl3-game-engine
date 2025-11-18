#include "engine.h"



Engine create_engine(){
    // Initialize all struct members to zero
    Engine e ; 
    e.run(&e) ; 

}


bool engine_init(Engine* engine) {
    SDL_zero(*engine) ; 
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
    engine->window = SDL_CreateWindow("OpenGL ES 2.0 Triangle",
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

void getevent(Engine* engine) {
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

void engine_swap(Engine* engine){
    SDL_GL_SwapWindow(engine->window);
}

void engine_zero(Engine* engine){
    engine->eventHandler = NULL;
    engine->draw = NULL;
    engine->on_engine_started = NULL;
    engine->on_engine_close = NULL;
    engine->on_engine_swap = NULL;
    engine->game = NULL;
    engine->window = NULL;
    engine->context = NULL;
    engine->run = 0 ; 
    engine->clearColor_r = 1.0f; 
    engine->clearColor_g = 0.6f; 
    engine->clearColor_b = 0.2f; 
}


int engine_run(Engine* engine) {

    if (!engine_start(engine)) {
        return 1;
    }
    if (engine->on_engine_started ) {
        engine->on_engine_started(engine);
    }
    
    while(engine->run){
        getevent(engine) ; 
        if (engine->eventHandler) {
            engine->eventHandler(engine) ; 
        }
        
        glClearColor(engine->clearColor_r, engine->clearColor_g, engine->clearColor_b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        if (engine->draw) {
            engine->draw(engine) ; 
        }
        if (engine->on_engine_swap) {
            engine->on_engine_swap(engine) ; 
        }
        engine_swap(engine) ; 
    }
    
    if (engine->on_engine_close) {
        engine->on_engine_close(engine) ; 
    }
    
    engine_destroy(engine);
    return 0;
}