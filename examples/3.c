#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_opengles2.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#define MAX_BLOCKS 5

/*------------------- Engine Structs -------------------*/
typedef struct { float x, y, z, w; } Vec4f;
typedef struct Engine Engine;
struct Engine {
    SDL_Window* window;
    SDL_GLContext context;
    Uint32 Windowflags;
    int width, height;
    SDL_Event event;
    int run;
    void (*eventHandler)(Engine*);
    void (*draw)(Engine*);
    void (*on_engine_started)(Engine*);
    void (*on_engine_closed)(Engine*);
    void (*on_engine_swaped)(Engine*);
    Vec4f clearcolor;
};

/*------------------- Engine Functions -------------------*/
bool engine_start(Engine* engine){
    SDL_zero(engine->event);
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0){
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return false;
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    if(!engine->Windowflags) engine->Windowflags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
    if(!engine->width) engine->width = 360;
    if(!engine->height) engine->height = 600;

    engine->window = SDL_CreateWindow("SDL3 OpenGL ES 2D Playground",
                                      engine->width, engine->height,
                                      engine->Windowflags);
    if(!engine->window){
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }

    engine->context = SDL_GL_CreateContext(engine->window);
    if(!engine->context){
        SDL_Log("SDL_GL_CreateContext failed: %s", SDL_GetError());
        SDL_DestroyWindow(engine->window);
        SDL_Quit();
        return false;
    }

    if(SDL_GL_MakeCurrent(engine->window, engine->context) < 0){
        SDL_Log("SDL_GL_MakeCurrent failed: %s", SDL_GetError());
        SDL_GL_DestroyContext(engine->context);
        SDL_DestroyWindow(engine->window);
        SDL_Quit();
        return false;
    }

    glViewport(0, 0, engine->width, engine->height);
    engine->run = 1;
    return true;
}

void engine_destroy(Engine* engine){
    if(engine->context) SDL_GL_DestroyContext(engine->context);
    if(engine->window) SDL_DestroyWindow(engine->window);
    SDL_Quit();
}

void engine_getevent(Engine* engine){
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        engine->event = event;
        if(event.type == SDL_EVENT_QUIT) engine->run = 0;
        if(event.type == SDL_EVENT_WINDOW_RESIZED){
            int w = event.window.data1;
            int h = event.window.data2;
            glViewport(0, 0, w, h);
        }
        if(event.type == SDL_EVENT_USER){
            printf("[CUSTOM EVENT] Collision detected!\n");
        }
    }
}

void engine_swap(Engine* engine){ SDL_GL_SwapWindow(engine->window); }
int engine_run(Engine* engine){
    if(!engine_start(engine)) return 1;
    if(engine->on_engine_started) engine->on_engine_started(engine);
    while(engine->run){
        engine_getevent(engine);
        if(engine->eventHandler) engine->eventHandler(engine);

        glClearColor(engine->clearcolor.x, engine->clearcolor.y, engine->clearcolor.z, engine->clearcolor.w);
        glClear(GL_COLOR_BUFFER_BIT);

        if(engine->draw) engine->draw(engine);

        if(engine->on_engine_swaped) engine->on_engine_swaped(engine);

        engine_swap(engine);
    }
    if(engine->on_engine_closed) engine->on_engine_closed(engine);
    engine_destroy(engine);
    return 0;
}

/*------------------- Game Code -------------------*/
typedef struct { float x, y, w, h; float speedX, speedY; } Block;

Block blocks[MAX_BLOCKS];
Block player;
float touchX=-1, touchY=-1;
float timeValue=0.0f;

GLuint shaderProgram, vbo;
Uint32 collisionEventType;

const char* vertexShaderSrc =
"attribute vec2 aPos;\n"
"void main(){\n"
"   gl_Position = vec4(aPos, 0.0, 1.0);\n"
"}\n";

const char* fragmentShaderSrc =
"precision mediump float;\n"
"uniform float uTime;\n"
"uniform vec2 uResolution;\n"
"void main(){\n"
"   vec2 st = gl_FragCoord.xy / uResolution;\n"
"   float r = 0.5 + 0.5*sin(uTime + st.x*10.0);\n"
"   float g = 0.5 + 0.5*sin(uTime + st.y*10.0);\n"
"   float b = 0.5 + 0.5*sin(uTime);\n"
"   gl_FragColor = vec4(r, g, b, 1.0);\n"
"}\n";

GLuint compileShader(GLenum type, const char* source){
    GLuint shader = glCreateShader(type);
    glShaderSource(shader,1,&source,NULL);
    glCompileShader(shader);
    GLint success; glGetShaderiv(shader, GL_COMPILE_STATUS,&success);
    if(!success){ char info[512]; glGetShaderInfoLog(shader,512,NULL,info); printf("Shader compile error: %s\n", info); }
    return shader;
}

void setupShaders(){
    GLuint vert = compileShader(GL_VERTEX_SHADER,vertexShaderSrc);
    GLuint frag = compileShader(GL_FRAGMENT_SHADER,fragmentShaderSrc);
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vert);
    glAttachShader(shaderProgram, frag);
    glLinkProgram(shaderProgram);
    glDeleteShader(vert); glDeleteShader(frag);
}

void setupBlocks(int width,int height){
    for(int i=0;i<MAX_BLOCKS;i++){
        blocks[i].w=50; blocks[i].h=50;
        blocks[i].x=rand() % (width-50);
        blocks[i].y=rand() % (height-50);
        blocks[i].speedX=(rand()%3+1)*((rand()%2)?1:-1);
        blocks[i].speedY=(rand()%3+1)*((rand()%2)?1:-1);
    }
}

float pxToNDCx(float x,int width){ return (x/(width/2))-1.0f; }
float pxToNDCy(float y,int height){ return 1.0f-(y/(height/2)); }

void engine_eventHandler(Engine* engine){
    const Uint8* state = SDL_GetKeyboardState(NULL);
    if(state[SDL_SCANCODE_UP]) player.y -= player.speedY;
    if(state[SDL_SCANCODE_DOWN]) player.y += player.speedY;
    if(state[SDL_SCANCODE_LEFT]) player.x -= player.speedX;
    if(state[SDL_SCANCODE_RIGHT]) player.x += player.speedX;

    SDL_Event e = engine->event;
    if(e.type==SDL_EVENT_FINGER_DOWN || e.type==SDL_EVENT_FINGER_MOTION){
        touchX=e.tfinger.x*engine->width;
        touchY=e.tfinger.y*engine->height;
        player.x=touchX-player.w/2;
        player.y=touchY-player.h/2;
    }
}

void engine_started(Engine* engine){
    player.x=engine->width/2-25;
    player.y=engine->height/2-25;
    player.w=50; player.h=50;
    player.speedX=5; player.speedY=5;

    setupShaders();
    setupBlocks(engine->width,engine->height);

    float vertices[]={-0.1f,-0.1f,0.1f,-0.1f,0.1f,0.1f,-0.1f,0.1f};
    glGenBuffers(1,&vbo);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_DYNAMIC_DRAW);

    collisionEventType = SDL_RegisterEvents(1); // register 1 custom event
}

void updateBlocks(Engine* engine){
    for(int i=0;i<MAX_BLOCKS;i++){
        blocks[i].x+=blocks[i].speedX;
        blocks[i].y+=blocks[i].speedY;
        if(blocks[i].x<0 || blocks[i].x+blocks[i].w>engine->width) blocks[i].speedX*=-1;
        if(blocks[i].y<0 || blocks[i].y+blocks[i].h>engine->height) blocks[i].speedY*=-1;

        // collision with player
        if(player.x < blocks[i].x+blocks[i].w && player.x+player.w>blocks[i].x &&
           player.y < blocks[i].y+blocks[i].h && player.y+player.h>blocks[i].y){
            blocks[i].speedX*=-1;
            blocks[i].speedY*=-1;

            // Trigger custom event
            SDL_Event ev;
            SDL_zero(ev);
            ev.type = collisionEventType;
            SDL_PushEvent(&ev);
        }
    }
}

void engine_draw(Engine* engine){
    timeValue+=0.05f;

    glUseProgram(shaderProgram);
    GLint posLoc=glGetAttribLocation(shaderProgram,"aPos");
    GLint timeLoc=glGetUniformLocation(shaderProgram,"uTime");
    GLint resLoc=glGetUniformLocation(shaderProgram,"uResolution");
    glUniform1f(timeLoc,timeValue);
    glUniform2f(resLoc,(float)engine->width,(float)engine->height);

    updateBlocks(engine);

    Block all[MAX_BLOCKS+1];
    all[0]=player;
    for(int i=0;i<MAX_BLOCKS;i++) all[i+1]=blocks[i];

    for(int i=0;i<=MAX_BLOCKS;i++){
        float x1=pxToNDCx(all[i].x,engine->width);
        float y1=pxToNDCy(all[i].y,engine->height);
        float x2=pxToNDCx(all[i].x+all[i].w,engine->width);
        float y2=pxToNDCy(all[i].y+all[i].h,engine->height);
        float vertices[]={x1,y1,x2,y1,x2,y2,x1,y2};
        glBindBuffer(GL_ARRAY_BUFFER,vbo);
        glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(posLoc);
        glVertexAttribPointer(posLoc,2,GL_FLOAT,GL_FALSE,0,0);
        glDrawArrays(GL_TRIANGLE_FAN,0,4);
    }
}

void on_engine_closed(Engine* engine){ printf("Engine closed\n"); }
void on_engine_swaped(Engine* engine){}

int main(int argc,char* argv[]){
    Engine engine; SDL_zero(engine);
    engine.Windowflags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
    engine.width=800; engine.height=600;
    engine.eventHandler=engine_eventHandler;
    engine.on_engine_started=engine_started;
    engine.draw=engine_draw;
    engine.on_engine_closed=on_engine_closed;
    engine.on_engine_swaped=on_engine_swaped;
    engine.clearcolor.x=0.1f; engine.clearcolor.y=0.1f;
    engine.clearcolor.z=0.1f; engine.clearcolor.w=1.0f;

    return engine_run(&engine);
}
