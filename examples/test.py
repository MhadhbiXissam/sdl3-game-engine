import os

# Mesh/pipeline metadata
pipeline_meta = {
    "name": "cube",
    "vertices": [
        # pos(x,y,z), normal(x,y,z), uv(u,v)
        [0.5, 0.5, 0.5, 0, 0, 1, 1, 1],
        [-0.5, 0.5, 0.5, 0, 0, 1, 0, 1],
        [-0.5, -0.5, 0.5, 0, 0, 1, 0, 0],
        [0.5, -0.5, 0.5, 0, 0, 1, 1, 0],
    ],
    "indices": [0, 1, 2, 0, 2, 3],
}

def generate_gles2_c(meta, folder="generated"):
    os.makedirs(folder, exist_ok=True)
    filename = os.path.join(folder, f"{meta['name']}.c")

    with open(filename, "w") as f:
        # Include GLES2 headers
        f.write("#include <GLES2/gl2.h>\n\n")

        # Vertex struct (for reference)
        f.write("typedef struct Vertex {\n")
        f.write("    float pos[3];\n    float normal[3];\n    float uv[2];\n")
        f.write("} Vertex;\n\n")

        # Flatten vertices for VBO
        f.write(f"const float vertices[] = {{\n")
        for v in meta["vertices"]:
            f.write(f"    {v[0]}, {v[1]}, {v[2]}, {v[3]}, {v[4]}, {v[5]}, {v[6]}, {v[7]},\n")
        f.write("};\n\n")

        # Indices for EBO
        f.write(f"const unsigned short indices[] = {{ {', '.join(map(str, meta['indices']))} }};\n\n")

        # Draw function
        f.write(f"void draw_{meta['name']}(GLuint program) {{\n")
        f.write("    GLuint VBO, EBO;\n")
        f.write("    glGenBuffers(1, &VBO);\n    glGenBuffers(1, &EBO);\n\n")
        f.write("    glBindBuffer(GL_ARRAY_BUFFER, VBO);\n")
        f.write("    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);\n\n")
        f.write("    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);\n")
        f.write("    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);\n\n")

        # Attribute pointers
        f.write("    GLint posLoc = glGetAttribLocation(program, \"aPos\");\n")
        f.write("    GLint normLoc = glGetAttribLocation(program, \"aNormal\");\n")
        f.write("    GLint uvLoc = glGetAttribLocation(program, \"aUV\");\n\n")

        f.write("    glEnableVertexAttribArray(posLoc);\n")
        f.write("    glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);\n")

        f.write("    glEnableVertexAttribArray(normLoc);\n")
        f.write("    glVertexAttribPointer(normLoc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));\n")

        f.write("    glEnableVertexAttribArray(uvLoc);\n")
        f.write("    glVertexAttribPointer(uvLoc, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));\n\n")

        # Draw call
        f.write(f"    glDrawElements(GL_TRIANGLES, {len(meta['indices'])}, GL_UNSIGNED_SHORT, 0);\n\n")

        f.write("    glDisableVertexAttribArray(posLoc);\n")
        f.write("    glDisableVertexAttribArray(normLoc);\n")
        f.write("    glDisableVertexAttribArray(uvLoc);\n\n")

        f.write("    glBindBuffer(GL_ARRAY_BUFFER, 0);\n")
        f.write("    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);\n")
        f.write("    glDeleteBuffers(1, &VBO);\n")
        f.write("    glDeleteBuffers(1, &EBO);\n")
        f.write("}\n")

    print(f"GLES2 C file generated: {filename}")

generate_gles2_c(pipeline_meta)
