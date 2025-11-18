#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_opengles2.h> // Include for OpenGL ES 2.0 functions

// Vertex Shader source code
const char* vertexShaderSource = R"(
attribute vec4 a_position;
void main() {
    gl_Position = a_position;
}
)";

// Fragment Shader source code
const char* fragmentShaderSource = R"(
precision mediump float;
void main() {
    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); // Red color
}
)";

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL could not initialize! SDL_Error: %s", SDL_GetError());
        return 1;
    }

    // Set OpenGL ES 2.0 attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // Create an SDL window with OpenGL ES context
    SDL_Window* window = SDL_CreateWindow("OpenGL ES 2.0 Triangle", 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window could not be created! SDL_Error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create an OpenGL ES context
    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "OpenGL ES context could not be created! SDL_Error: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Make the OpenGL ES context current
    if (SDL_GL_MakeCurrent(window, context) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to make OpenGL ES context current! SDL_Error: %s", SDL_GetError());
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Compile shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Create shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    // Define triangle vertices
    GLfloat vertices[] = {
        0.0f,  0.5f, 0.0f,
       -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f
    };

    // Create a Vertex Buffer Object (VBO)
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Get attribute location and enable it
    GLint posAttrib = glGetAttribLocation(shaderProgram, "a_position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    // Event loop
    SDL_Event event;
    int done = 0;
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                done = 1;
            }
        }

        // Clear the screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Greyish background
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw the triangle
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Swap buffers
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteBuffers(1, &vbo);
    SDL_GL_DestroyContext(context);

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}