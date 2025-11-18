#include <SDL3/SDL.h>
#include <SDL3/SDL_opengles2.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int score;

    // Square position
    float x, y;

    // OpenGL program and attributes
    GLuint prog;
    GLuint vbo;
    GLint attrPos;
    GLint uniColor;
    GLint uniOffset;  // Uniform for translation
} Game;

// --- Shader sources ---
const char* vertex_shader_src =
"attribute vec2 aPos;\n"
"uniform vec2 uOffset;\n"
"void main(){\n"
" gl_Position=vec4(aPos+uOffset,0.0,1.0);\n"
"}";

const char* fragment_shader_src =
"precision mediump float;\n"
"uniform vec4 uColor;\n"
"void main(){\n"
" gl_FragColor=uColor;\n"
"}";

// --- Shader helpers ---
GLuint compile_shader(GLenum type, const char* src){
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success){
        char buf[512];
        glGetShaderInfoLog(shader, 512, NULL, buf);
        printf("Shader compile error: %s\n", buf);
    }
    return shader;
}

GLuint create_program(const char* vs, const char* fs){
    GLuint vs_s = compile_shader(GL_VERTEX_SHADER, vs);
    GLuint fs_s = compile_shader(GL_FRAGMENT_SHADER, fs);
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs_s);
    glAttachShader(prog, fs_s);
    glLinkProgram(prog);
    GLint success;
    glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if (!success){
        char buf[512];
        glGetProgramInfoLog(prog, 512, NULL, buf);
        printf("Program link error: %s\n", buf);
    }
    glDeleteShader(vs_s);
    glDeleteShader(fs_s);
    return prog;
}

// --- Engine callbacks ---
void on_engine_started(void* e){
    Engine* engine = (Engine*)e;
    Game* game = (Game*)(engine->game);

    game->x = 0.0f;
    game->y = 0.0f;

    game->prog = create_program(vertex_shader_src, fragment_shader_src);
    game->attrPos = glGetAttribLocation(game->prog, "aPos");
    game->uniColor = glGetUniformLocation(game->prog, "uColor");
    game->uniOffset = glGetUniformLocation(game->prog, "uOffset");

    // Square vertex buffer (quad, size 0.2)
    GLfloat vertices[] = {
        -0.1f,-0.1f,
         0.1f,-0.1f,
         0.1f, 0.1f,
        -0.1f, 0.1f
    };
    glGenBuffers(1, &game->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, game->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    printf("Engine started!\n");
}

void draw(void* e){
    Engine* engine = (Engine*)e;
    Game* game = (Game*)(engine->game);

    glClearColor(engine->clearColor_r, engine->clearColor_g, engine->clearColor_b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(game->prog);

    glBindBuffer(GL_ARRAY_BUFFER, game->vbo);
    glEnableVertexAttribArray(game->attrPos);
    glVertexAttribPointer(game->attrPos, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);

    // Pass uniform translation and color
    glUniform2f(game->uniOffset, game->x, game->y);
    glUniform4f(game->uniColor, 0.8f, 0.3f, 0.2f, 1.0f);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableVertexAttribArray(game->attrPos);
}

void on_engine_swap(void* e){
    Engine* engine = (Engine*)e;
    Game* game = (Game*)(engine->game);

    game->score++;
    printf("Swap frame, score=%d\n", game->score);
}

void eventHandler(void* e){
    Engine* engine = (Engine*)e;
    Game* game = (Game*)(engine->game);

    // Use already captured event from engine
    SDL_Event ev = engine->event;

    if (ev.type == SDL_EVENT_QUIT){
        printf("Quit event\n");
    }

    if (ev.type == SDL_EVENT_KEY_DOWN){
        switch (ev.key.key){
            case SDLK_LEFT:  game->x -= 0.05f; break;
            case SDLK_RIGHT: game->x += 0.05f; break;
            case SDLK_UP:    game->y += 0.05f; break;
            case SDLK_DOWN:  game->y -= 0.05f; break;
        }
        printf("Square at (%f,%f)\n", game->x, game->y);
    }
}

void on_engine_close(void* e){
    Engine* engine = (Engine*)e;
    Game* game = (Game*)(engine->game);

    glDeleteProgram(game->prog);
    glDeleteBuffers(1, &game->vbo);

    printf("Engine closed!\n");
}

void engine_create(void* e){
    Engine* engine = (Engine*)e;
    engine->draw = draw;
    engine->on_engine_started = on_engine_started;
    engine->on_engine_swap = on_engine_swap;
    engine->on_engine_close = on_engine_close;
    engine->eventHandler = eventHandler;
    engine->game = (Game*)malloc(sizeof(Game));
    ((Game*)(engine->game))->score = 0;
}
