#include <GLES2/gl2.h>

typedef struct Vertex {
    float pos[3];
    float normal[3];
    float uv[2];
} Vertex;

const float vertices[] = {
    0.5, 0.5, 0.5, 0, 0, 1, 1, 1,
    -0.5, 0.5, 0.5, 0, 0, 1, 0, 1,
    -0.5, -0.5, 0.5, 0, 0, 1, 0, 0,
    0.5, -0.5, 0.5, 0, 0, 1, 1, 0,
};

const unsigned short indices[] = { 0, 1, 2, 0, 2, 3 };

void draw_cube(GLuint program) {
    GLuint VBO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    GLint posLoc = glGetAttribLocation(program, "aPos");
    GLint normLoc = glGetAttribLocation(program, "aNormal");
    GLint uvLoc = glGetAttribLocation(program, "aUV");

    glEnableVertexAttribArray(posLoc);
    glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(normLoc);
    glVertexAttribPointer(normLoc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(uvLoc);
    glVertexAttribPointer(uvLoc, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

    glDisableVertexAttribArray(posLoc);
    glDisableVertexAttribArray(normLoc);
    glDisableVertexAttribArray(uvLoc);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}
