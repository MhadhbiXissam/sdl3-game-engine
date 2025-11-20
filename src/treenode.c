#include <stdio.h>
#include <string.h>

typedef enum { ATTR_FLOAT, ATTR_INT, ATTR_CHAR, ATTR_STRUCT } AttrType;

// Nested structs
typedef struct { float x, y, z; } Vec3f;
typedef struct { float x, y, z, w; } Quatf;

// Metadata for each attribute
typedef struct {
    const char* name;   // Attribute name
    AttrType type;      // Type of data
    size_t size;        // Size in bytes (for array or struct)
    size_t offset;      // Offset in bytes, auto-calculated
} AttrMeta;

// Flattened camera memory
typedef struct {
    unsigned char data[64]; // total size (adjust as needed)
} CameraFlat;

// Define attributes with size; offset will be computed
AttrMeta camera_meta[] = {
    {"POSITION", ATTR_STRUCT, sizeof(Vec3f), 0},
    {"ROTATION", ATTR_STRUCT, sizeof(Quatf), 0},  // offset to compute
    {"TYPE", ATTR_INT, sizeof(int), 0},
    {"FOV", ATTR_FLOAT, sizeof(float), 0},
    {"ASPECT", ATTR_FLOAT, sizeof(float), 0},
    {"NEARZ", ATTR_FLOAT, sizeof(float), 0},
    {"FARZ", ATTR_FLOAT, sizeof(float), 0},
    {"ORTHO_SCALE", ATTR_FLOAT, sizeof(float), 0},
    {"NAME", ATTR_CHAR, 8, 0} // char[8] example
};

// Compute offsets dynamically
void compute_offsets() {
    size_t current = 0;
    for (int i = 0; i < sizeof(camera_meta)/sizeof(camera_meta[0]); i++) {
        camera_meta[i].offset = current;
        current += camera_meta[i].size;
    }
    printf("offset : %i\n" , current)  ;
}

// Generic getter
void* get_attr_ptr(CameraFlat* cam, int attr_index) {
    return (void*)(cam->data + camera_meta[attr_index].offset);
}

// Generic setter
void set_attr(CameraFlat* cam, int attr_index, void* value) {
    memcpy(get_attr_ptr(cam, attr_index), value, camera_meta[attr_index].size);
}

// Print attribute info
void print_attr(CameraFlat* cam, int attr_index) {
    AttrMeta meta = camera_meta[attr_index];
    printf("%s: ", meta.name);
    switch(meta.type) {
        case ATTR_FLOAT:
            printf("%f\n", *(float*)get_attr_ptr(cam, attr_index));
            break;
        case ATTR_INT:
            printf("%d\n", *(int*)get_attr_ptr(cam, attr_index));
            break;
        case ATTR_CHAR:
            printf("%s\n", (char*)get_attr_ptr(cam, attr_index));
            break;
        case ATTR_STRUCT:
            if (strcmp(meta.name, "POSITION") == 0) {
                Vec3f* v = (Vec3f*)get_attr_ptr(cam, attr_index);
                printf("%f %f %f\n", v->x, v->y, v->z);
            } else if (strcmp(meta.name, "ROTATION") == 0) {
                Quatf* q = (Quatf*)get_attr_ptr(cam, attr_index);
                printf("%f %f %f %f\n", q->x, q->y, q->z, q->w);
            }
            break;
    }
}

int main() {
    compute_offsets(); // calculate offsets automatically

    CameraFlat cam = {0};

    // Set attributes
    Vec3f pos = {1,2,3};
    Quatf rot = {0,0,0,1};
    int type = 1; // PERSPECTIVE
    float fov = 45.0f;
    char name[8] = "CAM1";

    set_attr(&cam, 0, &pos);
    set_attr(&cam, 1, &rot);
    set_attr(&cam, 2, &type);
    set_attr(&cam, 3, &fov);
    set_attr(&cam, 8, name);

    // Access and print
    print_attr(&cam, 0); // POSITION
    print_attr(&cam, 1); // ROTATION
    print_attr(&cam, 2); // TYPE
    print_attr(&cam, 3); // FOV
    print_attr(&cam, 8); // NAME

    // Direct memory access example: modify position x
    Vec3f* p = (Vec3f*)get_attr_ptr(&cam, 0);
    p->x = 10;
    print_attr(&cam, 0); // POSITION updated

    return 0;
}
