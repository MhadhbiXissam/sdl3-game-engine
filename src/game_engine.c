#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_opengles2.h> 
#include <time.h>
#include <stdbool.h>
// Macro shortcuts for SDL logging
#define LOG_LEVEL SDL_LOG_PRIORITY_VERBOSE
#define printInfo(msg, ...)  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, msg, ##__VA_ARGS__)
#define printWarn(msg, ...)  SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, msg, ##__VA_ARGS__)
#define printError(msg, ...) SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, msg, ##__VA_ARGS__)
#define printDebug(msg, ...) SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, msg, ##__VA_ARGS__)
#define printVerbose(msg, ...) SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, msg, ##__VA_ARGS__)
void game_engine_logHandler(void *userdata, int category, SDL_LogPriority priority, const char *message) {
   // ANSI color codes
   const char *RESET   = "\x1b[0m";
   const char *RED     = "\x1b[31m";
   const char *YELLOW  = "\x1b[33m";
   const char *GREEN   = "\x1b[32m";
   const char *BLUE    = "\x1b[34m";
   const char *MAGENTA = "\x1b[35m";
   const char *CYAN    = "\x1b[36m";
   const char *WHITE   = "\x1b[37m";

   // Static state to track last message and repeat count
   static char last_message[1024] = "";
   static int repeat_count = 0;

   // Map priority to string and color
   const char *level;
   const char *color;
   switch (priority) {
       case SDL_LOG_PRIORITY_VERBOSE: level = "VERBOSE"; color = CYAN; break;
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
   char time_buf[9]; // HH:MM:SS
   strftime(time_buf, sizeof(time_buf), "%H:%M:%S", tm_info);

   // Handle repeated messages
   if (strcmp(message, last_message) == 0) {
       repeat_count++;
       printf("\r%s[%s] [%s] %s (x%d)%s", color, time_buf, level, message, repeat_count + 1, RESET);
       fflush(stdout);
   } else {
       if (repeat_count > 0) printf("\n"); // finish previous repeated line
       repeat_count = 0;
       strncpy(last_message, message, sizeof(last_message) - 1);
       last_message[sizeof(last_message) - 1] = '\0';
       printf("%s[%s] [engine] [%s] %s%s\n", color, time_buf, level, message, RESET);
   }
}
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


typedef struct GameState GameState;
typedef GameState* (*ON_GAME_STARTED)(void);
static ON_GAME_STARTED PROC_ON_GAME_STARTED ; 
typedef void (*ON_GAME_UPDATE)(GameState*); 
static ON_GAME_UPDATE PROC_ON_GAME_UPDATE ; 
typedef void (*ON_GAME_EVENT)(GameState*,int); 
static ON_GAME_EVENT PROC_ON_GAME_EVENT ; 
typedef void (*ON_GAME_FINALIZE)(GameState*); 
static ON_GAME_FINALIZE PROC_ON_GAME_FINALIZE ; 
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
    void  (*on_engine_started)(Engine*) ; // call this method when window started 
    void (*on_engine_closed)(Engine*) ;  // call this method on engine before it closes 
    void (*on_engine_swaped)(Engine*) ;  // call this method before swap
    Vec4f clearcolor ; 
    void* gameloader ; 
    GameState* gamestate ; 
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
int  engine_run(Engine* engine) ; 
/*++++++++++++++++++++++++++++++++++++++++++++++*/
void engine_eventHandler(Engine* engine ); 
/*++++++++++++++++++++++++++++++++++++++++++++++*/
int engine_started(Engine* engine ); 
/*++++++++++++++++++++++++++++++++++++++++++++++*/
void engine_draw(Engine* engine ); 
/*++++++++++++++++++++++++++++++++++++++++++++++*/
void on_engine_closed(Engine* engine ); 
/*++++++++++++++++++++++++++++++++++++++++++++++*/
void on_engine_swaped(Engine* engine ); 
/*++++++++++++++++++++++++++++++++++++++++++++++*/

/*++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++++++++++++++++++++++++++++++*/

/*----------------------------------------------------------------------------------------------------------------------------------------------------*/
int main(int argc, char *argv[]){
    Engine engine;SDL_zero(engine);
    SDL_SetLogOutputFunction(game_engine_logHandler, NULL);SDL_SetLogPriorities(LOG_LEVEL);
    engine.Windowflags =  SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
    engine.width = 392 ; engine.height = 735 ; 
    engine.eventHandler = engine_eventHandler ; 
    engine.on_engine_started = engine_started ; 
    engine.draw = engine_draw ; 
    engine.on_engine_closed = on_engine_closed ; 
    engine.on_engine_swaped = on_engine_swaped ; 
    engine.clearcolor.x = 1.0 ; engine.clearcolor.y = 0.6 ; engine.clearcolor.z = 0.6 ; engine.clearcolor.w = 1.0 ; 
    engine_run(&engine);
    return 0;
}
/*----------------------------------------------------------------------------------------------------------------------------------------------------*/
bool engine_start(Engine* engine){
    SDL_zero(engine->event);
    // Initialize SDL with error checking
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printError("SDL_Init failed: %s", SDL_GetError());
        return false;
    }
    // Set GL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    if(!(engine->Windowflags)){
        engine->Windowflags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
    }
    if(!(engine->width)){
        engine->width = 360 ;
    }
    if(!(engine->height)){
        engine->height = 600 ;
    }
    // Create window with error checking
    engine->window = SDL_CreateWindow("OpenGL ES 2.0 ",
                                      engine->width, engine->height,
                                      engine->Windowflags);
    if (!engine->window) {
        printError("SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }
    // Create GL context
    engine->context = SDL_GL_CreateContext(engine->window);
    if (!engine->context) {
        printError("SDL_GL_CreateContext failed: %s", SDL_GetError());
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
    bool has_on_engine_swaped= false ; if (engine->on_engine_swaped) {has_on_engine_swaped = true ; }
    bool has_on_engine_closed= false ; if (engine->on_engine_closed) {has_on_engine_closed = true ; }
    while(engine->run){
        engine_getevent(engine) ; 
        if (has_eventHandler) {
            engine->eventHandler(engine) ; 
        }
        glClearColor(engine->clearcolor.x, engine->clearcolor.y, engine->clearcolor.z, engine->clearcolor.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (has_draw) {
            engine->draw(engine) ; 
        }
        if (has_on_engine_swaped) {
            engine->on_engine_swaped(engine) ; 
        }
        engine_swap(engine) ; 
    }
    
    if (has_on_engine_closed) {
        engine->on_engine_closed(engine) ; 
    }
    engine_destroy(engine);
    return 0;
}
//############################################
// hanndle captured event here 
void engine_eventHandler(Engine* engine ){
    SDL_Event e = engine->event ; 
    // process events here 
    PROC_ON_GAME_EVENT(engine->gamestate,e.type) ; 
}
//############################################
int  engine_started(Engine* engine ){
    printInfo("engine_started ..") ; 
    engine->gameloader = dlopen("src/libgame.so", RTLD_NOW);
    if (!(engine->gameloader)) {
        printError("%s\n", dlerror());
        dlclose(engine->gameloader);
        return -1 ; 
    }
    engine->gamestate = NULL ;

    PROC_ON_GAME_STARTED =  (ON_GAME_STARTED)dlsym(engine->gameloader, "on_game_started") ; 
    if(!PROC_ON_GAME_STARTED){printError("%s\n", dlerror());}else{
        engine->gamestate = PROC_ON_GAME_STARTED() ;

    }
    PROC_ON_GAME_UPDATE = (ON_GAME_UPDATE)dlsym(engine->gameloader, "on_game_update") ; 
    if(!PROC_ON_GAME_UPDATE){printError("%s\n", dlerror());}
    PROC_ON_GAME_FINALIZE = (ON_GAME_FINALIZE)dlsym(engine->gameloader, "on_game_finalize") ; 
    if(!PROC_ON_GAME_FINALIZE){printError("%s\n", dlerror());}
    PROC_ON_GAME_EVENT = (ON_GAME_EVENT)dlsym(engine->gameloader, "on_game_event") ; 
    if(!PROC_ON_GAME_EVENT){printError("%s\n", dlerror());}
    printDebug("here--------------");
    return 0 ; 
}
//############################################
void engine_draw(Engine* engine ){
    printInfo("engine_draw loop  ..\n") ; 
    if(PROC_ON_GAME_UPDATE){
        PROC_ON_GAME_UPDATE(engine->gamestate);
    }
}
//############################################
void on_engine_closed(Engine* engine ){
    typedef void (*ON_GAME_FINALIZE)(GameState*);
    //engine->gamestate = (GameState*)engine->gamestate ;
    PROC_ON_GAME_FINALIZE(engine->gamestate) ;
    if(engine->gameloader){
        dlclose(engine->gameloader);
    }
}
//############################################
void on_engine_swaped(Engine* engine ){
    printInfo("on_engine_swaped  ..\n") ; 
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