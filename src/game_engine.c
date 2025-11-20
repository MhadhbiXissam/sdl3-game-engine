// Enable GNU extensions for advanced features
#define _GNU_SOURCE
// Macro to bind a function pointer from a dynamic library with explicit type
#define BIND_LIB_FUNC_TYPED(lib_handle, func_ptr, func_name, type) \
    do {                                                           \
        type *tmp = (type *)dlsym(lib_handle, func_name);          \
        if (!tmp) {                                                \
            fprintf(stderr, "dlsym failed: %s\n", dlerror());     \
            return;                                                \
        }                                                          \
        *tmp = func_ptr;                                           \
    } while(0)

// Standard headers
#include <stdio.h>
#include <dlfcn.h>             // For dynamic library loading (dlopen, dlsym)
#include <time.h>
#include <stdbool.h>
#include <string.h>

// SDL3 headers for windowing, OpenGL ES, and events
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_opengles2.h>

// Logging configuration
#define LOG_LEVEL SDL_LOG_PRIORITY_DEBUG
#define printInfo(msg, ...)  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, msg, ##__VA_ARGS__)
#define printWarn(msg, ...)  SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, msg, ##__VA_ARGS__)
#define printError(msg, ...) SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, msg, ##__VA_ARGS__)
#define printDebug(msg, ...) SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, msg, ##__VA_ARGS__)
#define printVerbose(msg, ...) SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, msg, ##__VA_ARGS__)

/*---------------------------
   Custom SDL Logging Handler
---------------------------*/
void game_engine_logHandler(void *userdata, int category, SDL_LogPriority priority, const char *message) {
    // ANSI color codes for console output
    const char *RESET   = "\x1b[0m";
    const char *RED     = "\x1b[31m";
    const char *YELLOW  = "\x1b[33m";
    const char *GREEN   = "\x1b[32m";
    const char *BLUE    = "\x1b[34m";
    const char *MAGENTA = "\x1b[35m";
    const char *WHITE   = "\x1b[37m";

    static char last_message[1024] = ""; // store last message to detect repetition
    static int repeat_count = 0;         // repetition counter

    // Determine message level and color
    const char *level;
    const char *color;
    switch (priority) {
        case SDL_LOG_PRIORITY_VERBOSE: level = "VERBOSE"; color = BLUE; break;
        case SDL_LOG_PRIORITY_DEBUG:   level = "DEBUG";   color = BLUE; break;
        case SDL_LOG_PRIORITY_INFO:    level = "INFO";    color = GREEN; break;
        case SDL_LOG_PRIORITY_WARN:    level = "WARN";    color = YELLOW; break;
        case SDL_LOG_PRIORITY_ERROR:   level = "ERROR";   color = RED; break;
        case SDL_LOG_PRIORITY_CRITICAL:level = "CRITICAL"; color = MAGENTA; break;
        default: level = "UNKNOWN"; color = WHITE; break;
    }

    // Get current time
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char time_buf[9];
    strftime(time_buf, sizeof(time_buf), "%H:%M:%S", tm_info);

    // Handle repeated messages
    if (strcmp(message, last_message) == 0) {
        repeat_count++;
        printf("\r%s[%s] [%s] %s (x%d)%s", color, time_buf, level, message, repeat_count + 1, RESET);
        fflush(stdout);
    } else {
        if (repeat_count > 0) printf("\n");
        repeat_count = 0;
        strncpy(last_message, message, sizeof(last_message) - 1);
        last_message[sizeof(last_message) - 1] = '\0';
        printf("%s[%s] [engine] [%s] %s%s\n", color, time_buf, level, message, RESET);
    }
}

/*---------------------------
   Vector Structs
---------------------------*/
typedef struct { float x, y, z, w; } Vec4f;
typedef struct { float x, y, z; } Vec3f;
typedef struct { float x, y, z, w; } Vec4i;
typedef struct { float x, y, z; } Vec3i;

/*---------------------------
   Game Function Pointers
   Loaded dynamically from a game library
---------------------------*/
typedef void* (*ON_GAME_CREATED)(void);
typedef void  (*ON_GAME_INIT)(void*);
typedef void  (*ON_GAME_UPDATE)(void*);
typedef void  (*ON_GAME_EVENT)(void*);
typedef void  (*ON_GAME_FINALIZE)(void*);

static ON_GAME_CREATED PROC_ON_GAME_CREATED;
static ON_GAME_INIT    PROC_ON_GAME_INIT;
static ON_GAME_UPDATE  PROC_ON_GAME_UPDATE;
static ON_GAME_EVENT   PROC_ON_GAME_EVENT;
static ON_GAME_FINALIZE PROC_ON_GAME_FINALIZE;

/*---------------------------
   Engine Structure
---------------------------*/
typedef struct Engine {
    SDL_Window* window;
    SDL_GLContext context;
    Uint32 Windowflags;
    int width, height;
    SDL_Event event;
    int run;  // main loop flag

    // Engine callbacks
    void (*eventHandler)(void);
    void (*draw)(void);
    void (*on_engine_started)(void);
    void (*on_engine_closed)(void);
    void (*on_engine_swaped)(void);

    Vec4f clearcolor;   // background color
    void* gameloader;   // dynamic library handle
    void* gamestate;    // game state pointer
} Engine;

/* Global engine instance */
static Engine engine_instance;
static Engine* api = &engine_instance;

/*---------------------------
   Engine Function Declarations
---------------------------*/
bool engine_start(void);
void engine_destroy(void);
void engine_getevent(void);
void engine_swap(void);
int engine_run(void);
void engine_eventHandler(void);
int engine_started(void);
void engine_draw(void);
void on_engine_closed(void);
void on_engine_swaped(void);
void engine_api(void);
void get_window_size(int* w, int* h);

/*---------------------------
   Main Entry Point
---------------------------*/
int main(int argc, char *argv[]){
    SDL_zero(engine_instance);

    // Set custom logging
    SDL_SetLogOutputFunction(game_engine_logHandler, NULL);
    SDL_SetLogPriorities(LOG_LEVEL);

    // Configure engine defaults
    engine_instance.Windowflags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
    engine_instance.width = 392;
    engine_instance.height = 735;
    engine_instance.eventHandler = engine_eventHandler;
    engine_instance.on_engine_started = engine_started;
    engine_instance.draw = engine_draw;
    engine_instance.on_engine_closed = on_engine_closed;
    engine_instance.on_engine_swaped = on_engine_swaped;
    engine_instance.clearcolor = (Vec4f){1.0, 0.6, 0.6, 1.0};

    return engine_run();
}

/*---------------------------
   Engine Initialization
---------------------------*/
bool engine_start(void){
    SDL_zero(api->event);

    // Initialize SDL Video
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printError("SDL_Init failed: %s", SDL_GetError());
        return false;
    }

    // Configure OpenGL ES 2.0 context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // Default window size and flags
    if (!api->Windowflags) api->Windowflags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
    if (!api->width) api->width = 360;
    if (!api->height) api->height = 600;

    // Create SDL Window
    api->window = SDL_CreateWindow("OpenGL ES 2.0", api->width, api->height, api->Windowflags);
    if (!api->window) {
        printError("SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }

    // Create OpenGL context
    api->context = SDL_GL_CreateContext(api->window);
    if (!api->context) {
        printError("SDL_GL_CreateContext failed: %s", SDL_GetError());
        SDL_DestroyWindow(api->window);
        SDL_Quit();
        return false;
    }

    // Make context current
    if (SDL_GL_MakeCurrent(api->window, api->context) < 0) {
        SDL_Log("SDL_GL_MakeCurrent failed: %s", SDL_GetError());
        SDL_GL_DestroyContext(api->context);
        SDL_DestroyWindow(api->window);
        SDL_Quit();
        return false;
    }

    glViewport(0, 0, api->width, api->height);
    api->run = 1; 
    return true;
}

/*---------------------------
   Engine Cleanup
---------------------------*/
void engine_destroy(void){
    if (api->context) { SDL_GL_DestroyContext(api->context); api->context = NULL; }
    if (api->window) { SDL_DestroyWindow(api->window); api->window = NULL; }
    SDL_Quit();
}

/*---------------------------
   Event Handling
---------------------------*/
void engine_getevent(void){
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        api->event = event;
        if (event.type == SDL_EVENT_QUIT) api->run = 0;
        if (event.type == SDL_EVENT_WINDOW_RESIZED)
            glViewport(0, 0, event.window.data1, event.window.data2);
    }
}

/*---------------------------
   Buffer Swap
---------------------------*/
void engine_swap(void){ SDL_GL_SwapWindow(api->window); }

/*---------------------------
   Main Engine Loop
---------------------------*/
int engine_run(void){
    if (!engine_start()) return 1;
    if (api->on_engine_started) api->on_engine_started();

    bool has_eventHandler = api->eventHandler != NULL;
    bool has_draw = api->draw != NULL;
    bool has_on_engine_swaped = api->on_engine_swaped != NULL;
    bool has_on_engine_closed = api->on_engine_closed != NULL;

    while(api->run){
        engine_getevent();
        if (has_eventHandler) api->eventHandler();
        glClearColor(api->clearcolor.x, api->clearcolor.y, api->clearcolor.z, api->clearcolor.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (has_draw) api->draw();
        if (has_on_engine_swaped) api->on_engine_swaped();
        engine_swap();
    }

    if (has_on_engine_closed) api->on_engine_closed();
    engine_destroy();
    return 0;
}

/*---------------------------
   Game Engine Callbacks
---------------------------*/
void engine_eventHandler(void){ if(PROC_ON_GAME_EVENT) PROC_ON_GAME_EVENT(api->gamestate); }

int engine_started(void){
    printInfo("engine_started ..");

    // Load game library dynamically
    api->gameloader = dlopen("src/libgame.so", RTLD_NOW);
    if (!api->gameloader) {
        printError("%s\n", dlerror());
        dlclose(api->gameloader);
        return -1;
    }

    api->gamestate = NULL;

    // Load game functions dynamically
    PROC_ON_GAME_CREATED = (ON_GAME_CREATED)dlsym(api->gameloader, "on_game_created");
    if (PROC_ON_GAME_CREATED) api->gamestate = PROC_ON_GAME_CREATED();

    engine_api(); // set API callbacks

    PROC_ON_GAME_INIT = (ON_GAME_INIT)dlsym(api->gameloader, "on_game_init");
    if (PROC_ON_GAME_INIT) PROC_ON_GAME_INIT(api->gamestate);

    PROC_ON_GAME_UPDATE = (ON_GAME_UPDATE)dlsym(api->gameloader, "on_game_update");
    PROC_ON_GAME_FINALIZE = (ON_GAME_FINALIZE)dlsym(api->gameloader, "on_game_finalize");
    PROC_ON_GAME_EVENT = (ON_GAME_EVENT)dlsym(api->gameloader, "on_game_event");

    printDebug("creating game sys");
    return 0;
}

void engine_draw(void){ if (PROC_ON_GAME_UPDATE) PROC_ON_GAME_UPDATE(api->gamestate); }

void on_engine_closed(void){
    if(PROC_ON_GAME_FINALIZE) PROC_ON_GAME_FINALIZE(api->gamestate);
    if(api->gameloader) dlclose(api->gameloader);
}

void on_engine_swaped(void){ printInfo("on_engine_swaped .."); }

/*---------------------------
   Window Size Utility
---------------------------*/
void get_window_size(int* w, int* h){ 
    SDL_GetWindowSize(api->window, w, h); 
}

/*---------------------------
   Expose Engine API to Game Library
---------------------------*/
void engine_api(void){
    typedef void (*GetWindowSizeFunc)(int*, int*);
    BIND_LIB_FUNC_TYPED(api->gameloader, get_window_size, "get_window_size", GetWindowSizeFunc);


}
