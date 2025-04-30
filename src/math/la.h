#ifndef LA_H
#define LA_H

#include <stdint.h>
#include <math.h>

// vector types
typedef struct vec2 { float x, y; } vec2;
typedef struct vec3 { float x, y, z; } vec3; typedef struct vec4 { float x, y, z, w; } vec4;

// matrixtypes
typedef struct mat2 { float m[2][2]; } mat2;
typedef struct mat3 { float m[3][3]; } mat3;
typedef struct mat4 { float m[4][4]; } mat4;

// --------------------- Vector Operations ---------------------

// sum
#define vec_sum(a, b) _Generic((a),                     \
    vec2: vec2_sum,                                     \
    vec3: vec3_sum,                                     \
    vec4: vec4_sum                                      \
)(a, b)
static inline vec2 vec2_sum(vec2 a, vec2 b) { return (vec2){a.x + b.x, a.y + b.y}; }
static inline vec3 vec3_sum(vec3 a, vec3 b) { return (vec3){a.x + b.x, a.y + b.y, a.z + b.z}; }
static inline vec4 vec4_sum(vec4 a, vec4 b) { return (vec4){a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w}; }

// scale & negate
#define vec_scale(a, c) _Generic((a),                   \
    vec2: vec2_scale,                                   \
    vec3: vec3_scale,                                   \
    vec4: vec4_scale                                    \
)(a, c)
#define vec_negate(a) vec_scale(a, -1.0f)
static inline vec2 vec2_scale(vec2 a, float c) { return (vec2){a.x * c, a.y * c}; }
static inline vec3 vec3_scale(vec3 a, float c) { return (vec3){a.x * c, a.y * c, a.z * c}; }
static inline vec4 vec4_scale(vec4 a, float c) { return (vec4){a.x * c, a.y * c, a.z * c, a.w * c}; }

// dot product
#define vec_dot(a, b) _Generic((a),                     \
    vec2: vec2_dot,                                     \
    vec3: vec3_dot,                                     \
    vec4: vec4_dot                                      \
)(a, b)
static inline float vec2_dot(vec2 a, vec2 b) { return a.x*b.x + a.y*b.y; }
static inline float vec3_dot(vec3 a, vec3 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
static inline float vec4_dot(vec4 a, vec4 b) { return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w; }

// Length
#define vec_length(v) _Generic((v),                     \
    vec2: vec2_length,                                  \
    vec3: vec3_length,                                  \
    vec4: vec4_length                                   \
)(v)
static inline float vec2_length_sq(vec2 v) { return vec2_dot(v,v); }
static inline float vec2_length(vec2 v) { return sqrtf(vec2_length_sq(v)); }
static inline float vec3_length_sq(vec3 v) { return vec3_dot(v,v); }
static inline float vec3_length(vec3 v) { return sqrtf(vec3_length_sq(v)); }
static inline float vec4_length_sq(vec4 v) { return vec4_dot(v,v); }
static inline float vec4_length(vec4 v) { return sqrtf(vec4_length_sq(v)); }

// Normalize
#define vec_normalize(v) _Generic((v),                  \
    vec2: vec2_normalize,                               \
    vec3: vec3_normalize,                               \
    vec4: vec4_normalize                                \
)(v)
static inline vec2 vec2_normalize(vec2 v) { float len = vec2_length(v); return (len==0? (vec2){0,0} : (vec2){v.x/len,v.y/len}); }
static inline vec3 vec3_normalize(vec3 v) { float len = vec3_length(v); return (len==0? (vec3){0,0,0} : (vec3){v.x/len,v.y/len,v.z/len}); }
static inline vec4 vec4_normalize(vec4 v) { float len = vec4_length(v); return (len==0? (vec4){0,0,0,0} : (vec4){v.x/len,v.y/len,v.z/len,v.w/len}); }

// Distance
#define vec_distance(a, b) _Generic((a),                \
    vec2: vec2_distance,                                \
    vec3: vec3_distance,                                \
    vec4: vec4_distance                                 \
)(a, b)
static inline float vec2_distance(vec2 a, vec2 b) { return vec2_length((vec2){a.x-b.x, a.y-b.y}); }
static inline float vec3_distance(vec3 a, vec3 b) { return vec3_length((vec3){a.x-b.x, a.y-b.y, a.z-b.z}); }
static inline float vec4_distance(vec4 a, vec4 b) { return vec4_length((vec4){a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w}); }

// Component-wise multiply
#define vec_mult(a, b) _Generic((a),                    \
    vec2: vec2_mult,                                    \
    vec3: vec3_mult,                                    \
    vec4: vec4_mult                                     \
)(a, b)
static inline vec2 vec2_mult(vec2 a, vec2 b) { return (vec2){a.x*b.x, a.y*b.y}; }
static inline vec3 vec3_mult(vec3 a, vec3 b) { return (vec3){a.x*b.x, a.y*b.y, a.z*b.z}; }
static inline vec4 vec4_mult(vec4 a, vec4 b) { return (vec4){a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w}; }

// Min / Max
#define vec_min(a, b) _Generic((a),                     \
    vec2: vec2_min,                                     \
    vec3: vec3_min,                                     \
    vec4: vec4_min                                      \
)(a, b)
static inline vec2 vec2_min(vec2 a, vec2 b) { return (vec2){fminf(a.x,b.x), fminf(a.y,b.y)}; }
static inline vec3 vec3_min(vec3 a, vec3 b) { return (vec3){fminf(a.x,b.x), fminf(a.y,b.y), fminf(a.z,b.z)}; }
static inline vec4 vec4_min(vec4 a, vec4 b) { return (vec4){fminf(a.x,b.x), fminf(a.y,b.y), fminf(a.z,b.z), fminf(a.w,b.w)}; }

#define vec_max(a, b) _Generic((a),                     \
    vec2: vec2_max,                                     \
    vec3: vec3_max,                                     \
    vec4: vec4_max                                      \
)(a, b)
static inline vec2 vec2_max(vec2 a, vec2 b) { return (vec2){fmaxf(a.x,b.x), fmaxf(a.y,b.y)}; }
static inline vec3 vec3_max(vec3 a, vec3 b) { return (vec3){fmaxf(a.x,b.x), fmaxf(a.y,b.y), fmaxf(a.z,b.z)}; }
static inline vec4 vec4_max(vec4 a, vec4 b) { return (vec4){fmaxf(a.x,b.x), fmaxf(a.y,b.y), fmaxf(a.z,b.z), fmaxf(a.w,b.w)}; }

// Lerp
#define vec_lerp(a, b, t) _Generic((a),                 \
    vec2: vec2_lerp,                                    \
    vec3: vec3_lerp,                                    \
    vec4: vec4_lerp                                     \
)(a, b, t)
static inline vec2 vec2_lerp(vec2 a, vec2 b, float t) { return (vec2){a.x + (b.x-a.x)*t, a.y + (b.y-a.y)*t}; }
static inline vec3 vec3_lerp(vec3 a, vec3 b, float t) { return (vec3){a.x + (b.x-a.x)*t, a.y + (b.y-a.y)*t, a.z + (b.z-a.z)*t}; }
static inline vec4 vec4_lerp(vec4 a, vec4 b, float t) { return (vec4){a.x + (b.x-a.x)*t, a.y + (b.y-a.y)*t, a.z + (b.z-a.z)*t, a.w + (b.w-a.w)*t}; }

// Clamp
#define vec_clamp(v, lo, hi) _Generic((v),              \
    vec2: vec2_clamp,                                   \
    vec3: vec3_clamp,                                   \
    vec4: vec4_clamp                                    \
)(v, lo, hi)
static inline vec2 vec2_clamp(vec2 v, vec2 lo, vec2 hi) { return (vec2){fminf(fmaxf(v.x, lo.x), hi.x), 
                                                                        fminf(fmaxf(v.y, lo.y), hi.y)}; }
static inline vec3 vec3_clamp(vec3 v, vec3 lo, vec3 hi) { return (vec3){fminf(fmaxf(v.x, lo.x), hi.x),
                                                                        fminf(fmaxf(v.y, lo.y), hi.y),
                                                                        fminf(fmaxf(v.z, lo.z), hi.z)}; }
static inline vec4 vec4_clamp(vec4 v, vec4 lo, vec4 hi) { return (vec4){fminf(fmaxf(v.x, lo.x), hi.x), 
                                                                        fminf(fmaxf(v.y, lo.y), hi.y),
                                                                        fminf(fmaxf(v.z, lo.z), hi.z),
                                                                        fminf(fmaxf(v.w, lo.w), hi.w)}; }

// Reflect / Refract (only for vec2, vec3)
#define vec_reflect(I, N) _Generic((I),                 \
    vec2: vec2_reflect,                                 \
    vec3: vec3_reflect                                  \
)(I, N)
static inline vec2 vec2_reflect(vec2 I, vec2 N) { float d = vec2_dot(I, N); return (vec2){I.x - 2*d*N.x, I.y - 2*d*N.y}; }
static inline vec3 vec3_reflect(vec3 I, vec3 N) { float d = vec3_dot(I, N); return (vec3){I.x - 2*d*N.x, I.y - 2*d*N.y, I.z - 2*d*N.z}; }

#define vec_refract(I, N, eta) _Generic((I),            \
    vec2: vec2_refract,                                 \
    vec3: vec3_refract                                  \
)(I, N, eta)
static inline vec2 vec2_refract(vec2 I, vec2 N, float eta) {
    float cosi = -fminf(fmaxf(vec2_dot(I, N), -1.0f), 1.0f);
    float k    = 1.0f - eta*eta*(1.0f - cosi*cosi);
    if (k < 0.0f) return (vec2){0,0};
    float a    = eta*cosi - sqrtf(k);
    return (vec2){eta*I.x + a*N.x, eta*I.y + a*N.y};
}
static inline vec3 vec3_refract(vec3 I, vec3 N, float eta) {
    float cosi = -fminf(fmaxf(vec3_dot(I, N), -1.0f), 1.0f);
    float k    = 1.0f - eta*eta*(1.0f - cosi*cosi);
    if (k < 0.0f) return (vec3){0,0,0};
    float a    = eta*cosi - sqrtf(k);
    return (vec3){eta*I.x + a*N.x, eta*I.y + a*N.y, eta*I.z + a*N.z};
}

// Cross product (vec3 only)
static inline vec3 vec_cross(vec3 a, vec3 b) {
    return (vec3){a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x};
}

// --------------------- Matrix Operations ---------------------

// Multiply
#define mat_mul(a, b) _Generic((a),                     \
    mat2: mat2_mult,                                    \
    mat3: mat3_mult,                                    \
    mat4: mat4_mult                                     \
)(a, b)
static inline mat2 mat2_mult(mat2 a, mat2 b) {
    return (mat2){ .m = {
        {a.m[0][0]*b.m[0][0] + a.m[0][1]*b.m[1][0], a.m[0][0]*b.m[0][1] + a.m[0][1]*b.m[1][1]},
        {a.m[1][0]*b.m[0][0] + a.m[1][1]*b.m[1][0], a.m[1][0]*b.m[0][1] + a.m[1][1]*b.m[1][1]}
    }};
}
static inline mat3 mat3_mult(mat3 a, mat3 b) {
    return (mat3){ .m = {
            { a.m[0][0]*b.m[0][0] + a.m[0][1]*b.m[1][0] + a.m[0][2]*b.m[2][0],
              a.m[0][0]*b.m[0][1] + a.m[0][1]*b.m[1][1] + a.m[0][2]*b.m[2][1],
              a.m[0][0]*b.m[0][2] + a.m[0][1]*b.m[1][2] + a.m[0][2]*b.m[2][2] },

            { a.m[1][0]*b.m[0][0] + a.m[1][1]*b.m[1][0] + a.m[1][2]*b.m[2][0],
              a.m[1][0]*b.m[0][1] + a.m[1][1]*b.m[1][1] + a.m[1][2]*b.m[2][1],
              a.m[1][0]*b.m[0][2] + a.m[1][1]*b.m[1][2] + a.m[1][2]*b.m[2][2] },

            { a.m[2][0]*b.m[0][0] + a.m[2][1]*b.m[1][0] + a.m[2][2]*b.m[2][0],
              a.m[2][0]*b.m[0][1] + a.m[2][1]*b.m[1][1] + a.m[2][2]*b.m[2][1],
              a.m[2][0]*b.m[0][2] + a.m[2][1]*b.m[1][2] + a.m[2][2]*b.m[2][2] }
        }
    };
}
static inline mat4 mat4_mult(const mat4 a, const mat4 b) {
    return (mat4){ .m = {
        { a.m[0][0]*b.m[0][0] + a.m[0][1]*b.m[1][0] + a.m[0][2]*b.m[2][0] + a.m[0][3]*b.m[3][0],
          a.m[0][0]*b.m[0][1] + a.m[0][1]*b.m[1][1] + a.m[0][2]*b.m[2][1] + a.m[0][3]*b.m[3][1],
          a.m[0][0]*b.m[0][2] + a.m[0][1]*b.m[1][2] + a.m[0][2]*b.m[2][2] + a.m[0][3]*b.m[3][2],
          a.m[0][0]*b.m[0][3] + a.m[0][1]*b.m[1][3] + a.m[0][2]*b.m[2][3] + a.m[0][3]*b.m[3][3] },

        { a.m[1][0]*b.m[0][0] + a.m[1][1]*b.m[1][0] + a.m[1][2]*b.m[2][0] + a.m[1][3]*b.m[3][0],
          a.m[1][0]*b.m[0][1] + a.m[1][1]*b.m[1][1] + a.m[1][2]*b.m[2][1] + a.m[1][3]*b.m[3][1],
          a.m[1][0]*b.m[0][2] + a.m[1][1]*b.m[1][2] + a.m[1][2]*b.m[2][2] + a.m[1][3]*b.m[3][2],
          a.m[1][0]*b.m[0][3] + a.m[1][1]*b.m[1][3] + a.m[1][2]*b.m[2][3] + a.m[1][3]*b.m[3][3] },

        { a.m[2][0]*b.m[0][0] + a.m[2][1]*b.m[1][0] + a.m[2][2]*b.m[2][0] + a.m[2][3]*b.m[3][0],
          a.m[2][0]*b.m[0][1] + a.m[2][1]*b.m[1][1] + a.m[2][2]*b.m[2][1] + a.m[2][3]*b.m[3][1],
          a.m[2][0]*b.m[0][2] + a.m[2][1]*b.m[1][2] + a.m[2][2]*b.m[2][2] + a.m[2][3]*b.m[3][2],
          a.m[2][0]*b.m[0][3] + a.m[2][1]*b.m[1][3] + a.m[2][2]*b.m[2][3] + a.m[2][3]*b.m[3][3] },

        { a.m[3][0]*b.m[0][0] + a.m[3][1]*b.m[1][0] + a.m[3][2]*b.m[2][0] + a.m[3][3]*b.m[3][0],
          a.m[3][0]*b.m[0][1] + a.m[3][1]*b.m[1][1] + a.m[3][2]*b.m[2][1] + a.m[3][3]*b.m[3][1],
          a.m[3][0]*b.m[0][2] + a.m[3][1]*b.m[1][2] + a.m[3][2]*b.m[2][2] + a.m[3][3]*b.m[3][2],
          a.m[3][0]*b.m[0][3] + a.m[3][1]*b.m[1][3] + a.m[3][2]*b.m[2][3] + a.m[3][3]*b.m[3][3] }
    }};
}

// Matrix-vector multiply
#define mat_vec_mul(a, v) _Generic((a),                 \
    mat2: mat2_vec_mult,                                \
    mat3: mat3_vec_mult,                                \
    mat4: mat4_vec_mult                                 \
)(a, v)
static inline vec2 mat2_vec_mult(mat2 m, vec2 v) { return (vec2){m.m[0][0]*v.x + m.m[0][1]*v.y, m.m[1][0]*v.x + m.m[1][1]*v.y}; }
static inline vec3 mat3_vec_mult(mat3 m, vec3 v) { return (vec3){ m.m[0][0]*v.x + m.m[0][1]*v.y + m.m[0][2]*v.z,
                                                                  m.m[1][0]*v.x + m.m[1][1]*v.y + m.m[1][2]*v.z,
                                                                  m.m[2][0]*v.x + m.m[2][1]*v.y + m.m[2][2]*v.z };
}
static inline vec4 mat4_vec_mult(mat4 m, vec4 v) { return (vec4){ m.m[0][0]*v.x + m.m[0][1]*v.y + m.m[0][2]*v.z + m.m[0][3]*v.w,
                                                                  m.m[1][0]*v.x + m.m[1][1]*v.y + m.m[1][2]*v.z + m.m[1][3]*v.w,
                                                                  m.m[2][0]*v.x + m.m[2][1]*v.y + m.m[2][2]*v.z + m.m[2][3]*v.w,
                                                                  m.m[3][0]*v.x + m.m[3][1]*v.y + m.m[3][2]*v.z + m.m[3][3]*v.w };
}

// Transpose
#define mat_transpose(m) _Generic((m),                  \
    mat2: mat2_transpose,                               \
    mat3: mat3_transpose,                               \
    mat4: mat4_transpose                                \
)(m)
static inline mat2 mat2_transpose(mat2 m) { return (mat2){ .m = {{m.m[0][0], m.m[1][0]}, {m.m[0][1], m.m[1][1]}} }; }
static inline mat3 mat3_transpose(mat3 m) {
    mat3 o;
    for (int i=0; i<3; i++) for (int j=0; j<3; j++) o.m[i][j] = m.m[j][i];
    return o;
}
static inline mat4 mat4_transpose(mat4 m) {
    mat4 o;
    for (int i=0; i<4; i++) for (int j=0; j<4; j++) o.m[i][j] = m.m[j][i];
    return o;
}

// Identity
static inline mat2 mat2_identity(void) { return (mat2){ .m = {{1,0},{0,1}} }; }
static inline mat3 mat3_identity(void) { return (mat3){ .m = {{1,0,0},{0,1,0},{0,0,1}} }; }
static inline mat4 mat4_identity(void) { return (mat4){ .m = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}} }; }

// Uniform scale
#define mat_scale_uniform(m, s) _Generic((m),           \
    mat2: mat2_scale_uniform,                           \
    mat3: mat3_scale_uniform,                           \
    mat4: mat4_scale_uniform                            \
)(m, s)
static inline mat2 mat2_scale_uniform(mat2 m, float s) { return (mat2){ .m = {{m.m[0][0]*s, m.m[0][1]*s}, {m.m[1][0]*s, m.m[1][1]*s}} }; }
static inline mat3 mat3_scale_uniform(mat3 m, float s) {
    mat3 o;
    for (int i=0; i<3; i++) for (int j=0; j<3; j++) o.m[i][j] = m.m[i][j] * s;
    return o;
}
static inline mat4 mat4_scale_uniform(mat4 m, float s) {
    mat4 o = m;
    for (int i=0; i<4; i++) for (int j=0; j<4; j++) o.m[i][j] *= s;
    return o;
}

// Determinant
#define mat_determinant(m) _Generic((m),                \
    mat2: mat2_determinant,                             \
    mat3: mat3_determinant,                             \
    mat4: mat4_determinant                              \
)(m)
static inline float mat2_determinant(mat2 m) { return m.m[0][0]*m.m[1][1] - m.m[0][1]*m.m[1][0]; }
static inline float mat3_determinant(mat3 m) {
    return m.m[0][0]*(m.m[1][1]*m.m[2][2] - m.m[1][2]*m.m[2][1])
         - m.m[0][1]*(m.m[1][0]*m.m[2][2] - m.m[1][2]*m.m[2][0])
         + m.m[0][2]*(m.m[1][0]*m.m[2][1] - m.m[1][1]*m.m[2][0]);
}
static inline float mat4_determinant(mat4 m) {
    #define M(i,j) m.m[i][j]
    float det =
        M(0,0)*(M(1,1)*(M(2,2)*M(3,3)-M(2,3)*M(3,2)) - M(1,2)*(M(2,1)*M(3,3)-M(2,3)*M(3,1)) + M(1,3)*(M(2,1)*M(3,2)-M(2,2)*M(3,1)))
      - M(0,1)*(M(1,0)*(M(2,2)*M(3,3)-M(2,3)*M(3,2)) - M(1,2)*(M(2,0)*M(3,3)-M(2,3)*M(3,0)) + M(1,3)*(M(2,0)*M(3,2)-M(2,2)*M(3,0)))
      + M(0,2)*(M(1,0)*(M(2,1)*M(3,3)-M(2,3)*M(3,1)) - M(1,1)*(M(2,0)*M(3,3)-M(2,3)*M(3,0)) + M(1,3)*(M(2,0)*M(3,1)-M(2,1)*M(3,0)))
      - M(0,3)*(M(1,0)*(M(2,1)*M(3,2)-M(2,2)*M(3,1)) - M(1,1)*(M(2,0)*M(3,2)-M(2,2)*M(3,0)) + M(1,2)*(M(2,0)*M(3,1)-M(2,1)*M(3,0)));
    #undef M
    return det;
}

// Inverse
#define mat_inverse(m) _Generic((m),                    \
    mat2: mat2_inverse,                                 \
    mat3: mat3_inverse,                                 \
    mat4: mat4_inverse                                  \
)(m)
static inline mat2 mat2_inverse(mat2 m) {
    float det = mat2_determinant(m);
    if (det == 0.0f) return (mat2){0};
    float invDet = 1.0f / det;
    return (mat2){ .m = {{ m.m[1][1]*invDet, -m.m[0][1]*invDet}, {-m.m[1][0]*invDet, m.m[0][0]*invDet}} };
}
static inline mat3 mat3_inverse(mat3 m) {
    float det = mat3_determinant(m);
    if (det == 0.0f) return (mat3){0};
    float invDet = 1.0f / det;
    mat3 inv;
    inv.m[0][0] = +(m.m[1][1]*m.m[2][2] - m.m[1][2]*m.m[2][1]) * invDet;
    inv.m[0][1] = -(m.m[0][1]*m.m[2][2] - m.m[0][2]*m.m[2][1]) * invDet;
    inv.m[0][2] = +(m.m[0][1]*m.m[1][2] - m.m[0][2]*m.m[1][1]) * invDet;
    inv.m[1][0] = -(m.m[1][0]*m.m[2][2] - m.m[1][2]*m.m[2][0]) * invDet;
    inv.m[1][1] = +(m.m[0][0]*m.m[2][2] - m.m[0][2]*m.m[2][0]) * invDet;
    inv.m[1][2] = -(m.m[0][0]*m.m[1][2] - m.m[0][2]*m.m[1][0]) * invDet;
    inv.m[2][0] = +(m.m[1][0]*m.m[2][1] - m.m[1][1]*m.m[2][0]) * invDet;
    inv.m[2][1] = -(m.m[0][0]*m.m[2][1] - m.m[0][1]*m.m[2][0]) * invDet;
    inv.m[2][2] = +(m.m[0][0]*m.m[1][1] - m.m[0][1]*m.m[1][0]) * invDet;
    return inv;
}
static inline mat4 mat4_inverse(mat4 m) {
    mat4 inv;
    // compute all 16 cofactors (from known expansion)
    inv.m[0][0] =  m.m[1][1]*m.m[2][2]*m.m[3][3] - m.m[1][1]*m.m[2][3]*m.m[3][2]
                  -m.m[2][1]*m.m[1][2]*m.m[3][3] + m.m[2][1]*m.m[1][3]*m.m[3][2]
                  +m.m[3][1]*m.m[1][2]*m.m[2][3] - m.m[3][1]*m.m[1][3]*m.m[2][2];
    inv.m[0][1] = -m.m[0][1]*m.m[2][2]*m.m[3][3] + m.m[0][1]*m.m[2][3]*m.m[3][2]
                  +m.m[2][1]*m.m[0][2]*m.m[3][3] - m.m[2][1]*m.m[0][3]*m.m[3][2]
                  -m.m[3][1]*m.m[0][2]*m.m[2][3] + m.m[3][1]*m.m[0][3]*m.m[2][2];
    inv.m[0][2] =  m.m[0][1]*m.m[1][2]*m.m[3][3] - m.m[0][1]*m.m[1][3]*m.m[3][2]
                  -m.m[1][1]*m.m[0][2]*m.m[3][3] + m.m[1][1]*m.m[0][3]*m.m[3][2]
                  +m.m[3][1]*m.m[0][2]*m.m[1][3] - m.m[3][1]*m.m[0][3]*m.m[1][2];
    inv.m[0][3] = -m.m[0][1]*m.m[1][2]*m.m[2][3] + m.m[0][1]*m.m[1][3]*m.m[2][2]
                  +m.m[1][1]*m.m[0][2]*m.m[2][3] - m.m[1][1]*m.m[0][3]*m.m[2][2]
                  -m.m[2][1]*m.m[0][2]*m.m[1][3] + m.m[2][1]*m.m[0][3]*m.m[1][2];

    inv.m[1][0] = -m.m[1][0]*m.m[2][2]*m.m[3][3] + m.m[1][0]*m.m[2][3]*m.m[3][2]
                  +m.m[2][0]*m.m[1][2]*m.m[3][3] - m.m[2][0]*m.m[1][3]*m.m[3][2]
                  -m.m[3][0]*m.m[1][2]*m.m[2][3] + m.m[3][0]*m.m[1][3]*m.m[2][2];
    inv.m[1][1] =  m.m[0][0]*m.m[2][2]*m.m[3][3] - m.m[0][0]*m.m[2][3]*m.m[3][2]
                  -m.m[2][0]*m.m[0][2]*m.m[3][3] + m.m[2][0]*m.m[0][3]*m.m[3][2]
                  +m.m[3][0]*m.m[0][2]*m.m[2][3] - m.m[3][0]*m.m[0][3]*m.m[2][2];
    inv.m[1][2] = -m.m[0][0]*m.m[1][2]*m.m[3][3] + m.m[0][0]*m.m[1][3]*m.m[3][2]
                  +m.m[1][0]*m.m[0][2]*m.m[3][3] - m.m[1][0]*m.m[0][3]*m.m[3][2]
                  -m.m[3][0]*m.m[0][2]*m.m[1][3] + m.m[3][0]*m.m[0][3]*m.m[1][2];
    inv.m[1][3] =  m.m[0][0]*m.m[1][2]*m.m[2][3] - m.m[0][0]*m.m[1][3]*m.m[2][2]
                  -m.m[1][0]*m.m[0][2]*m.m[2][3] + m.m[1][0]*m.m[0][3]*m.m[2][2]
                  +m.m[2][0]*m.m[0][2]*m.m[1][3] - m.m[2][0]*m.m[0][3]*m.m[1][2];

    inv.m[2][0] =  m.m[1][0]*m.m[2][1]*m.m[3][3] - m.m[1][0]*m.m[2][3]*m.m[3][1]
                  -m.m[2][0]*m.m[1][1]*m.m[3][3] + m.m[2][0]*m.m[1][3]*m.m[3][1]
                  +m.m[3][0]*m.m[1][1]*m.m[2][3] - m.m[3][0]*m.m[1][3]*m.m[2][1];
    inv.m[2][1] = -m.m[0][0]*m.m[2][1]*m.m[3][3] + m.m[0][0]*m.m[2][3]*m.m[3][1]
                  +m.m[2][0]*m.m[0][1]*m.m[3][3] - m.m[2][0]*m.m[0][3]*m.m[3][1]
                  -m.m[3][0]*m.m[0][1]*m.m[2][3] + m.m[3][0]*m.m[0][3]*m.m[2][1];
    inv.m[2][2] =  m.m[0][0]*m.m[1][1]*m.m[3][3] - m.m[0][0]*m.m[1][3]*m.m[3][1]
                  -m.m[1][0]*m.m[0][1]*m.m[3][3] + m.m[1][0]*m.m[0][3]*m.m[3][1]
                  +m.m[3][0]*m.m[0][1]*m.m[1][3] - m.m[3][0]*m.m[0][3]*m.m[1][1];
    inv.m[2][3] = -m.m[0][0]*m.m[1][1]*m.m[2][3] + m.m[0][0]*m.m[1][3]*m.m[2][1]
                  +m.m[1][0]*m.m[0][1]*m.m[2][3] - m.m[1][0]*m.m[0][3]*m.m[2][1]
                  -m.m[2][0]*m.m[0][1]*m.m[1][3] + m.m[2][0]*m.m[0][3]*m.m[1][1];

    inv.m[3][0] = -m.m[1][0]*m.m[2][1]*m.m[3][2] + m.m[1][0]*m.m[2][2]*m.m[3][1]
                  +m.m[2][0]*m.m[1][1]*m.m[3][2] - m.m[2][0]*m.m[1][2]*m.m[3][1]
                  -m.m[3][0]*m.m[1][1]*m.m[2][2] + m.m[3][0]*m.m[1][2]*m.m[2][1];
    inv.m[3][1] =  m.m[0][0]*m.m[2][1]*m.m[3][2] - m.m[0][0]*m.m[2][2]*m.m[3][1]
                  -m.m[2][0]*m.m[0][1]*m.m[3][2] + m.m[2][0]*m.m[0][2]*m.m[3][1]
                  +m.m[3][0]*m.m[0][1]*m.m[2][2] - m.m[3][0]*m.m[0][2]*m.m[2][1];
    inv.m[3][2] = -m.m[0][0]*m.m[1][1]*m.m[3][2] + m.m[0][0]*m.m[1][2]*m.m[3][1]
                  +m.m[1][0]*m.m[0][1]*m.m[3][2] - m.m[1][0]*m.m[0][2]*m.m[3][1]
                  -m.m[3][0]*m.m[0][1]*m.m[1][2] + m.m[3][0]*m.m[0][2]*m.m[1][1];
    inv.m[3][3] =  m.m[0][0]*m.m[1][1]*m.m[2][2] - m.m[0][0]*m.m[1][2]*m.m[2][1]
                  -m.m[1][0]*m.m[0][1]*m.m[2][2] + m.m[1][0]*m.m[0][2]*m.m[2][1]
                  +m.m[2][0]*m.m[0][1]*m.m[1][2] - m.m[2][0]*m.m[0][2]*m.m[1][1];

    float det = mat4_determinant(m);
    float invDet = 1.0f / det;

    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            inv.m[i][j] *= invDet;

    return inv;
}

#endif // LA_H

