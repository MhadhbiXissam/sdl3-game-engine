#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_opengles2.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define MAX_NODES 10
#define DEG2RAD(a) ((a) * 0.0174533f)

/*---------------- Node & Scene ----------------*/
typedef struct Node Node;
typedef struct Scene Scene;

struct Node {
    float x, y;
    float rotation;
    float scaleX, scaleY;
    int renderable;
    int rotate;
    float rotation_speed;
    float moveSpeedX, moveSpeedY;
    float moveDirX, moveDirY;
    float color[3];
    Node* children[MAX_NODES];
    int child_count;
};

struct Scene {
    Node* root;
    float rotation;
};

/*---------------- Game & Engine ----------------*/
typedef struct {
    int state;
    Scene** scenes;
    int scene_count;
    int current_scene_index;
} Game;

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
    Game* game;
    float clearcolor[4];
    GLuint shaderProgram;
    GLuint vbo;
};

/*---------------- Matrix Helper ----------------*/
void mat4_identity(float* m){
    for(int i=0;i<16;i++) m[i]=0.0f;
    m[0]=1; m[5]=1; m[10]=1; m[15]=1;
}

void mat4_translate(float* m, float x, float y, float z){
    m[12]+=x; m[13]+=y; m[14]+=z;
}

void mat4_scale(float* m, float sx, float sy, float sz){
    m[0]*=sx; m[5]*=sy; m[10]*=sz;
}

void mat4_rotate_z(float* m, float angleDeg){
    float rad = angleDeg * 0.0174533f;
    float c=cosf(rad), s=sinf(rad);
    float m0=m[0], m1=m[1], m4=m[4], m5=m[5];
    m[0]=c*m0+s*m1; m[1]=-s*m0+c*m1;
    m[4]=c*m4+s*m5; m[5]=-s*m4+c*m5;
}

/*---------------- Shader ----------------*/
const char* vertexShaderSrc =
"attribute vec2 aPos;\n"
"uniform mat4 uModel;\n"
"void main(){ gl_Position=uModel*vec4(aPos,0.0,1.0); }\n";

const char* fragmentShaderSrc =
"precision mediump float;\n"
"uniform vec3 uColor;\n"
"void main(){ gl_FragColor=vec4(uColor,1.0); }\n";

GLuint compileShader(GLenum type,const char* src){
    GLuint shader=glCreateShader(type);
    glShaderSource(shader,1,&src,NULL);
    glCompileShader(shader);
    GLint success; glGetShaderiv(shader,GL_COMPILE_STATUS,&success);
    if(!success){ char info[512]; glGetShaderInfoLog(shader,512,NULL,info); printf("Shader error: %s\n",info);}
    return shader;
}

void setupShaders(Engine* engine){
    GLuint vert=compileShader(GL_VERTEX_SHADER,vertexShaderSrc);
    GLuint frag=compileShader(GL_FRAGMENT_SHADER,fragmentShaderSrc);
    engine->shaderProgram=glCreateProgram();
    glAttachShader(engine->shaderProgram,vert);
    glAttachShader(engine->shaderProgram,frag);
    glLinkProgram(engine->shaderProgram);
    glDeleteShader(vert); glDeleteShader(frag);

    float vertices[]={-0.05f,-0.05f,0.05f,-0.05f,0.05f,0.05f,-0.05f,0.05f};
    glGenBuffers(1,&engine->vbo);
    glBindBuffer(GL_ARRAY_BUFFER,engine->vbo);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
}

/*---------------- Node Tree ----------------*/
Node* create_node(float x,float y,int renderable,int rotate,float speed,float sx,float sy,float r,float g,float b){
    Node* n=(Node*)malloc(sizeof(Node));
    n->x=x; n->y=y; n->rotation=0;
    n->scaleX=sx; n->scaleY=sy;
    n->renderable=renderable; n->rotate=rotate; n->rotation_speed=speed;
    n->moveSpeedX=0.002f; n->moveSpeedY=0.002f; n->moveDirX=1; n->moveDirY=1;
    n->color[0]=r; n->color[1]=g; n->color[2]=b;
    n->child_count=0; for(int i=0;i<MAX_NODES;i++) n->children[i]=NULL;
    return n;
}

void add_child(Node* parent,Node* child){ if(parent->child_count<MAX_NODES) parent->children[parent->child_count++]=child; }

void animate_node(Node* node){
    if(!node) return;
    if(node->rotate) node->rotation+=node->rotation_speed;

    // Simple translation animation back/forth
    node->x += node->moveSpeedX * node->moveDirX;
    node->y += node->moveSpeedY * node->moveDirY;
    if(node->x>0.5f || node->x<-0.5f) node->moveDirX*=-1;
    if(node->y>0.5f || node->y<-0.5f) node->moveDirY*=-1;

    for(int i=0;i<node->child_count;i++) animate_node(node->children[i]);
}

void render_node(Engine* engine,Node* node,float* parentMat){
    if(!node) return;
    float model[16]; for(int i=0;i<16;i++) model[i]=parentMat[i];
    mat4_translate(model,node->x,node->y,0);
    mat4_rotate_z(model,node->rotation);
    mat4_scale(model,node->scaleX,node->scaleY,1.0f);

    GLuint modelLoc=glGetUniformLocation(engine->shaderProgram,"uModel");
    glUniformMatrix4fv(modelLoc,1,GL_FALSE,model);

    if(node->renderable){
        GLuint posLoc=glGetAttribLocation(engine->shaderProgram,"aPos");
        glEnableVertexAttribArray(posLoc);
        glBindBuffer(GL_ARRAY_BUFFER,engine->vbo);
        glVertexAttribPointer(posLoc,2,GL_FLOAT,GL_FALSE,0,0);

        GLuint colorLoc=glGetUniformLocation(engine->shaderProgram,"uColor");
        glUniform3f(colorLoc,node->color[0],node->color[1],node->color[2]);
        glDrawArrays(GL_TRIANGLE_FAN,0,4);
    }

    for(int i=0;i<node->child_count;i++) render_node(engine,node->children[i],model);
}

void render_scene(Engine* engine,Scene* scene){
    float model[16]; for(int i=0;i<16;i++) model[i]=(i%5==0)?1.0f:0.0f;
    animate_node(scene->root);
    render_node(engine,scene->root,model);
}

/*---------------- Game ----------------*/
Game* create_game(){ Game* g=(Game*)malloc(sizeof(Game)); g->state=0; g->scenes=NULL; g->scene_count=0; g->current_scene_index=0; return g; }
void add_scene(Game* g,Scene* s){ g->scenes=(Scene**)realloc(g->scenes,sizeof(Scene*)*(g->scene_count+1)); g->scenes[g->scene_count++]=s; }
Scene* get_current_scene(Game* g){ if(g->scene_count==0) return NULL; return g->scenes[g->current_scene_index]; }

/*---------------- Engine Callbacks ----------------*/
void engine_eventHandler(Engine* engine){
    const Uint8* state=SDL_GetKeyboardState(NULL);
    Scene* s=get_current_scene(engine->game);
    if(!s) return;

    if(state[SDL_SCANCODE_LEFT]) s->rotation-=1.0f;
    if(state[SDL_SCANCODE_RIGHT]) s->rotation+=1.0f;
    printf("Scene rotation: %.2f\n",s->rotation);

    if(state[SDL_SCANCODE_A]) s->root->rotate=1;
    if(state[SDL_SCANCODE_S] && s->root->child_count>0) s->root->children[0]->rotate=1;
    if(state[SDL_SCANCODE_D] && s->root->child_count>1) s->root->children[1]->rotate=1;
}

void engine_draw(Engine* engine){
    Scene* s=get_current_scene(engine->game);
    glUseProgram(engine->shaderProgram);
    if(s) render_scene(engine,s);
}

void engine_started(Engine* engine){
    setupShaders(engine);
    Node* root=create_node(0.0f,0.0f,0,0,0,1.0f,1.0f,1.0f,1.0f,1.0f);
    Node* c1=create_node(0.3f,0.0f,1,0,2.0f,0.5f,0.5f,1,0,0);
    Node* c2=create_node(-0.3f,0.0f,1,0,1.5f,0.5f,0.5f,0,1,0);
    Node* gc=create_node(0.0f,0.2f,1,0,3.0f,0.3f,0.3f,0,0,1);
    add_child(c1,gc);
    add_child(root,c1);
    add_child(root,c2);

    Scene* scene=(Scene*)malloc(sizeof(Scene));
    scene->root=root; scene->rotation=0;
    add_scene(engine->game,scene);

    engine->clearcolor[0]=0.1f; engine->clearcolor[1]=0.1f; engine->clearcolor[2]=0.1f; engine->clearcolor[3]=1.0f;
}

void on_engine_closed(Engine* engine){ printf("Engine closed\n"); }
void on_engine_swaped(Engine* engine){}

/*---------------- Engine Core ----------------*/
bool engine_start(Engine* engine){
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS)<0){ printf("SDL_Init failed: %s\n",SDL_GetError()); return false; }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
    engine->window=SDL_CreateWindow("GLES2 Scene",engine->width,engine->height,engine->Windowflags);
    if(!engine->window){ printf("SDL_CreateWindow failed\n"); SDL_Quit(); return false; }
    engine->context=SDL_GL_CreateContext(engine->window);
    if(!engine->context){ printf("SDL_GL_CreateContext failed\n"); SDL_DestroyWindow(engine->window); SDL_Quit(); return false; }
    SDL_GL_MakeCurrent(engine->window,engine->context);
    glViewport(0,0,engine->width,engine->height);
    engine->run=1;
    return true;
}

void engine_destroy(Engine* engine){
    if(engine->context) SDL_GL_DestroyContext(engine->context);
    if(engine->window) SDL_DestroyWindow(engine->window);
    SDL_Quit();
}

void engine_getevent(Engine* engine){
    SDL_Event e;
    while(SDL_PollEvent(&e)){
        engine->event=e;
        if(e.type==SDL_EVENT_QUIT) engine->run=0;
    }
}

void engine_swap(Engine* engine){ SDL_GL_SwapWindow(engine->window); }

int engine_run(Engine* engine){
    if(!engine_start(engine)) return 1;
    if(engine->on_engine_started) engine->on_engine_started(engine);
    while(engine->run){
        engine_getevent(engine);
        if(engine->eventHandler) engine->eventHandler(engine);
        glClearColor(engine->clearcolor[0],engine->clearcolor[1],engine->clearcolor[2],engine->clearcolor[3]);
        glClear(GL_COLOR_BUFFER_BIT);
        if(engine->draw) engine->draw(engine);
        if(engine->on_engine_swaped) engine->on_engine_swaped(engine);
        engine_swap(engine);
    }
    if(engine->on_engine_closed) engine->on_engine_closed(engine);
    engine_destroy(engine);
    return 0;
}

/*---------------- Main ----------------*/
int main(int argc,char* argv[]){
    Engine engine; SDL_zero(engine);
    engine.Windowflags=SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE;
    engine.width=800; engine.height=600;
    engine.eventHandler=engine_eventHandler;
    engine.draw=engine_draw;
    engine.on_engine_started=engine_started;
    engine.on_engine_closed=on_engine_closed;
    engine.on_engine_swaped=on_engine_swaped;
    engine.game=create_game();
    return engine_run(&engine);
}
