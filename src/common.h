#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// ======== Vector Types ========
typedef struct { float x, y, z; } Vec3f;
typedef struct { float x, y, z, w; } Vec4f;
typedef struct { int x, y, z; } Vec3i;
typedef struct { int x, y, z, w; } Vec4i;

// ======== Matrix Types ========
typedef struct { float m[4][4]; } Mat4f;
typedef struct { float m[3][3]; } Mat3f;

// ======== Quaternion Type ========
typedef struct { float x, y, z, w; } Quatf;

// ======== Vec3f Operations ========
static inline Vec3f vec3f_add(Vec3f a, Vec3f b) { return (Vec3f){a.x+b.x, a.y+b.y, a.z+b.z}; }
static inline Vec3f vec3f_sub(Vec3f a, Vec3f b) { return (Vec3f){a.x-b.x, a.y-b.y, a.z-b.z}; }
static inline Vec3f vec3f_scale(Vec3f v, float s) { return (Vec3f){v.x*s, v.y*s, v.z*s}; }
static inline float vec3f_dot(Vec3f a, Vec3f b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
static inline Vec3f vec3f_cross(Vec3f a, Vec3f b) {
    return (Vec3f){a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x};
}
static inline float vec3f_length(Vec3f v) { return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z); }
static inline Vec3f vec3f_normalize(Vec3f v) {
    float len = vec3f_length(v);
    return len > 0.0f ? vec3f_scale(v, 1.0f/len) : v;
}

// ======== Vec4f Operations ========
static inline Vec4f vec4f_add(Vec4f a, Vec4f b) { return (Vec4f){a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w}; }
static inline Vec4f vec4f_sub(Vec4f a, Vec4f b) { return (Vec4f){a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w}; }
static inline Vec4f vec4f_scale(Vec4f v, float s) { return (Vec4f){v.x*s, v.y*s, v.z*s, v.w*s}; }
static inline float vec4f_dot(Vec4f a, Vec4f b) { return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w; }
static inline float vec4f_length(Vec4f v) { return sqrtf(vec4f_dot(v,v)); }
static inline Vec4f vec4f_normalize(Vec4f v) {
    float len = vec4f_length(v);
    return len > 0.0f ? vec4f_scale(v, 1.0f/len) : v;
}

// ======== Integer Vectors ========
static inline Vec3i vec3i_add(Vec3i a, Vec3i b) { return (Vec3i){a.x+b.x, a.y+b.y, a.z+b.z}; }
static inline Vec3i vec3i_sub(Vec3i a, Vec3i b) { return (Vec3i){a.x-b.x, a.y-b.y, a.z-b.z}; }
static inline Vec4i vec4i_add(Vec4i a, Vec4i b) { return (Vec4i){a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w}; }
static inline Vec4i vec4i_sub(Vec4i a, Vec4i b) { return (Vec4i){a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w}; }

// ======== Mat4f Operations ========
static inline Mat4f mat4f_identity() {
    Mat4f m = {0};
    m.m[0][0] = m.m[1][1] = m.m[2][2] = m.m[3][3] = 1.0f;
    return m;
}

static inline Mat4f mat4f_mul(Mat4f a, Mat4f b) {
    Mat4f r = {0};
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            for(int k=0;k<4;k++)
                r.m[i][j] += a.m[i][k]*b.m[k][j];
    return r;
}

static inline Vec4f mat4f_mul_vec4f(Mat4f m, Vec4f v) {
    return (Vec4f){
        m.m[0][0]*v.x + m.m[0][1]*v.y + m.m[0][2]*v.z + m.m[0][3]*v.w,
        m.m[1][0]*v.x + m.m[1][1]*v.y + m.m[1][2]*v.z + m.m[1][3]*v.w,
        m.m[2][0]*v.x + m.m[2][1]*v.y + m.m[2][2]*v.z + m.m[2][3]*v.w,
        m.m[3][0]*v.x + m.m[3][1]*v.y + m.m[3][2]*v.z + m.m[3][3]*v.w
    };
}

static inline Mat4f mat4f_translate(float x, float y, float z) {
    Mat4f m = mat4f_identity();
    m.m[0][3] = x; m.m[1][3] = y; m.m[2][3] = z;
    return m;
}

static inline Mat4f mat4f_scale_mat(float x, float y, float z) {
    Mat4f m = mat4f_identity();
    m.m[0][0] = x; m.m[1][1] = y; m.m[2][2] = z;
    return m;
}

static inline Mat4f mat4f_rotate_x(float angle) {
    Mat4f m = mat4f_identity();
    float c = cosf(angle), s = sinf(angle);
    m.m[1][1]=c; m.m[1][2]=-s; m.m[2][1]=s; m.m[2][2]=c;
    return m;
}

static inline Mat4f mat4f_rotate_y(float angle) {
    Mat4f m = mat4f_identity();
    float c = cosf(angle), s = sinf(angle);
    m.m[0][0]=c; m.m[0][2]=s; m.m[2][0]=-s; m.m[2][2]=c;
    return m;
}

static inline Mat4f mat4f_rotate_z(float angle) {
    Mat4f m = mat4f_identity();
    float c = cosf(angle), s = sinf(angle);
    m.m[0][0]=c; m.m[0][1]=-s; m.m[1][0]=s; m.m[1][1]=c;
    return m;
}

// ======== Mat3f Operations ========
static inline Mat3f mat3f_identity() {
    Mat3f m = {0};
    m.m[0][0]=m.m[1][1]=m.m[2][2]=1.0f;
    return m;
}

static inline Vec3f mat3f_mul_vec3f(Mat3f m, Vec3f v) {
    return (Vec3f){
        m.m[0][0]*v.x + m.m[0][1]*v.y + m.m[0][2]*v.z,
        m.m[1][0]*v.x + m.m[1][1]*v.y + m.m[1][2]*v.z,
        m.m[2][0]*v.x + m.m[2][1]*v.y + m.m[2][2]*v.z
    };
}

// ======== Quaternion Operations ========
static inline Quatf quat_identity() { return (Quatf){0,0,0,1}; }

static inline Quatf quat_from_axis_angle(Vec3f axis, float angle) {
    float half = angle*0.5f;
    float s = sinf(half);
    axis = vec3f_normalize(axis);
    return (Quatf){axis.x*s, axis.y*s, axis.z*s, cosf(half)};
}

static inline Quatf quat_mul(Quatf a, Quatf b) {
    return (Quatf){
        a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y,
        a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x,
        a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w,
        a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z
    };
}

static inline Mat4f quat_to_mat4f(Quatf q) {
    Mat4f m = mat4f_identity();
    float xx=q.x*q.x, yy=q.y*q.y, zz=q.z*q.z;
    float xy=q.x*q.y, xz=q.x*q.z, yz=q.y*q.z;
    float wx=q.w*q.x, wy=q.w*q.y, wz=q.w*q.z;
    m.m[0][0]=1-2*(yy+zz); m.m[0][1]=2*(xy-wz); m.m[0][2]=2*(xz+wy);
    m.m[1][0]=2*(xy+wz); m.m[1][1]=1-2*(xx+zz); m.m[1][2]=2*(yz-wx);
    m.m[2][0]=2*(xz-wy); m.m[2][1]=2*(yz+wx); m.m[2][2]=1-2*(xx+yy);
    return m;
}

// ======== Projection Matrices ========
static inline Mat4f mat4f_perspective(float fovy, float aspect, float nearZ, float farZ) {
    Mat4f m = {0};
    float f = 1.0f/tanf(fovy*0.5f);
    m.m[0][0] = f/aspect;
    m.m[1][1] = f;
    m.m[2][2] = (farZ+nearZ)/(nearZ-farZ);
    m.m[2][3] = (2*farZ*nearZ)/(nearZ-farZ);
    m.m[3][2] = -1;
    return m;
}

static inline Mat4f mat4f_orthographic(float left, float right, float bottom, float top, float nearZ, float farZ) {
    Mat4f m = mat4f_identity();
    m.m[0][0] = 2.0f/(right-left);
    m.m[1][1] = 2.0f/(top-bottom);
    m.m[2][2] = -2.0f/(farZ-nearZ);
    m.m[0][3] = -(right+left)/(right-left);
    m.m[1][3] = -(top+bottom)/(top-bottom);
    m.m[2][3] = -(farZ+nearZ)/(farZ-nearZ);
    return m;
}

// ======== Camera (LookAt) ========
static inline Mat4f mat4f_look_at(Vec3f eye, Vec3f center, Vec3f up) {
    Vec3f f = vec3f_normalize(vec3f_sub(center, eye));
    Vec3f s = vec3f_normalize(vec3f_cross(f, up));
    Vec3f u = vec3f_cross(s, f);

    Mat4f m = mat4f_identity();
    m.m[0][0]=s.x; m.m[0][1]=s.y; m.m[0][2]=s.z;
    m.m[1][0]=u.x; m.m[1][1]=u.y; m.m[1][2]=u.z;
    m.m[2][0]=-f.x; m.m[2][1]=-f.y; m.m[2][2]=-f.z;
    m.m[0][3]=-vec3f_dot(s, eye);
    m.m[1][3]=-vec3f_dot(u, eye);
    m.m[2][3]=vec3f_dot(f, eye);
    return m;
}

// ======== Utility Functions ========
static inline Vec3f vec3f_lerp(Vec3f a, Vec3f b, float t) {
    return vec3f_add(vec3f_scale(a,1-t), vec3f_scale(b,t));
}

static inline Quatf quat_slerp(Quatf a, Quatf b, float t) {
    float dot = a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
    if(dot < 0.0f) { b.x=-b.x; b.y=-b.y; b.z=-b.z; b.w=-b.w; dot=-dot; }
    if(dot > 0.9995f) return quat_from_axis_angle(vec3f_normalize((Vec3f){b.x-a.x, b.y-a.y, b.z-a.z}), t);
    float theta_0 = acosf(dot), theta = theta_0*t;
    float sin_theta = sinf(theta), sin_theta_0 = sinf(theta_0);
    float s0 = cosf(theta) - dot*sin_theta/sin_theta_0;
    float s1 = sin_theta/sin_theta_0;
    return (Quatf){a.x*s0+b.x*s1, a.y*s0+b.y*s1, a.z*s0+b.z*s1, a.w*s0+b.w*s1};
}
