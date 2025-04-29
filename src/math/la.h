#ifndef LA_H
#define LA_H
#include <stdint.h>
#include <math.h>
#define SM (CLS / sizeof (double))

typedef struct vec2 {
    float x; 
    float y;
} vec2;

typedef struct vec3 {
    float x; 
    float y; 
    float z;
} vec3;

typedef struct vec4 {
    float x; 
    float y; 
    float z; 
    float w;
} vec4;
typedef struct mat2 {
    float m[2][2];
} mat2;

typedef struct mat3 {
    float m[3][3];
} mat3;

typedef struct mat4 {
    float m[4][4];
} mat4;

// @vec_sum
#define vec_sum(a, b) _Generic((a), \
    vec2: vec2_sum,                 \
    vec3: vec3_sum,                 \
    vec4: vec4_sum                  \
)(a, b)

static inline vec2 vec2_sum(vec2 a, vec2 b) {
    vec2 out = {
        .x = a.x + b.x, 
        .y = a.y + b.y
    }; 
    return out;
}

static inline vec3 vec3_sum(vec3 a, vec3 b) {
    vec3 out = {
        .x = a.x + b.x, 
        .y = a.y + b.y,
        .z = a.z + b.z
    }; 
    return out;
}

static inline vec4 vec4_sum(vec4 a, vec4 b) {
    vec4 out = {
        .x = a.x + b.x, 
        .y = a.y + b.y,
        .z = a.z + b.z,
        .w = a.w + b.w
    }; 
    return out;
}

// @vec_scale
#define vec_negate(a) _Generic((a),     \
    vec2: vector2_scale,                \
    vec3: vector3_scale,                \
    vec4: vector4_scale                 \
)(a, -1.0)

#define vec_scale(a, c) _Generic((a),   \
    vec2: vector2_scale,                \
    vec3: vector3_scale,                \
    vec4: vector4_scale,                \
)(a, c)

static inline vec2 vector2_scale(vec2 a, float c) {
    vec2 out = {
        .x = a.x * c, 
        .y = a.y * c 
    };
    return out;
}

static inline vec3 vector3_scale(vec3 a, float c) {
    vec3 out = {
        .x = a.x * c, 
        .y = a.y * c,
        .z = a.z * c
    }; 
    return out;
}

static inline vec4 vector4_scale(vec4 a, float c) {
    vec4 out = {
        .x = a.x * c, 
        .y = a.y * c,
        .z = a.z * c,
        .w = a.w * c
    }; 
    return out;
}

// @mat_mul
#define mat_mul(a, b) _Generic((a),     \
    mat2: mat2_mult,                    \
    mat3: mat3_mult,                    \
    mat4: mat4_mult                     \
)(a, b)

static inline mat2 mat2_mult(mat2 a, mat2 b) {
    mat2 out = {
        .m ={
            { 
                (a.m[0][0] * b.m[0][0]) + (a.m[0][1] * b.m[1][0]),
                (a.m[0][0] * b.m[0][1]) + (a.m[0][1] * b.m[1][1]) 
            },
            { 
                (a.m[1][0] * b.m[0][0]) + (a.m[1][1] * b.m[1][0]), 
                (a.m[1][0] * b.m[0][1]) + (a.m[1][1] * b.m[1][1])
            }
        }
    };
    return out;
}

static inline mat3 mat3_mult(mat3 a, mat3 b) {
    mat3 out = {
        .m = {
            {
                a.m[0][0]*b.m[0][0] + a.m[0][1]*b.m[1][0] + a.m[0][2]*b.m[2][0],
                a.m[0][0]*b.m[0][1] + a.m[0][1]*b.m[1][1] + a.m[0][2]*b.m[2][1],
                a.m[0][0]*b.m[0][2] + a.m[0][1]*b.m[1][2] + a.m[0][2]*b.m[2][2]
            },
            {
                a.m[1][0]*b.m[0][0] + a.m[1][1]*b.m[1][0] + a.m[1][2]*b.m[2][0],
                a.m[1][0]*b.m[0][1] + a.m[1][1]*b.m[1][1] + a.m[1][2]*b.m[2][1],
                a.m[1][0]*b.m[0][2] + a.m[1][1]*b.m[1][2] + a.m[1][2]*b.m[2][2]
            },
            {
                a.m[2][0]*b.m[0][0] + a.m[2][1]*b.m[1][0] + a.m[2][2]*b.m[2][0],
                a.m[2][0]*b.m[0][1] + a.m[2][1]*b.m[1][1] + a.m[2][2]*b.m[2][1],
                a.m[2][0]*b.m[0][2] + a.m[2][1]*b.m[1][2] + a.m[2][2]*b.m[2][2]
            }
        }
    };
    return out;
}

static inline mat4 mat4_mult(mat4 a, mat4 b) {
    mat4 out = {
        .m = {
            {
                a.m[0][0]*b.m[0][0] + a.m[0][1]*b.m[1][0] + a.m[0][2]*b.m[2][0] + a.m[0][3]*b.m[3][0],
                a.m[0][0]*b.m[0][1] + a.m[0][1]*b.m[1][1] + a.m[0][2]*b.m[2][1] + a.m[0][3]*b.m[3][1],
                a.m[0][0]*b.m[0][2] + a.m[0][1]*b.m[1][2] + a.m[0][2]*b.m[2][2] + a.m[0][3]*b.m[3][2],
                a.m[0][0]*b.m[0][3] + a.m[0][1]*b.m[1][3] + a.m[0][2]*b.m[2][3] + a.m[0][3]*b.m[3][3]
            },
            {
                a.m[1][0]*b.m[0][0] + a.m[1][1]*b.m[1][0] + a.m[1][2]*b.m[2][0] + a.m[1][3]*b.m[3][0],
                a.m[1][0]*b.m[0][1] + a.m[1][1]*b.m[1][1] + a.m[1][2]*b.m[2][1] + a.m[1][3]*b.m[3][1],
                a.m[1][0]*b.m[0][2] + a.m[1][1]*b.m[1][2] + a.m[1][2]*b.m[2][2] + a.m[1][3]*b.m[3][2],
                a.m[1][0]*b.m[0][3] + a.m[1][1]*b.m[1][3] + a.m[1][2]*b.m[2][3] + a.m[1][3]*b.m[3][3]
            },
            {
                a.m[2][0]*b.m[0][0] + a.m[2][1]*b.m[1][0] + a.m[2][2]*b.m[2][0] + a.m[2][3]*b.m[3][0],
                a.m[2][0]*b.m[0][1] + a.m[2][1]*b.m[1][1] + a.m[2][2]*b.m[2][1] + a.m[2][3]*b.m[3][1],
                a.m[2][0]*b.m[0][2] + a.m[2][1]*b.m[1][2] + a.m[2][2]*b.m[2][2] + a.m[2][3]*b.m[3][2],
                a.m[2][0]*b.m[0][3] + a.m[2][1]*b.m[1][3] + a.m[2][2]*b.m[2][3] + a.m[2][3]*b.m[3][3]
            },
            {
                a.m[3][0]*b.m[0][0] + a.m[3][1]*b.m[1][0] + a.m[3][2]*b.m[2][0] + a.m[3][3]*b.m[3][0],
                a.m[3][0]*b.m[0][1] + a.m[3][1]*b.m[1][1] + a.m[3][2]*b.m[2][1] + a.m[3][3]*b.m[3][1],
                a.m[3][0]*b.m[0][2] + a.m[3][1]*b.m[1][2] + a.m[3][2]*b.m[2][2] + a.m[3][3]*b.m[3][2],
                a.m[3][0]*b.m[0][3] + a.m[3][1]*b.m[1][3] + a.m[3][2]*b.m[2][3] + a.m[3][3]*b.m[3][3]
            }
        }
    };
    return out;
}

// @mat_vec_mul
#define mat_vec_mul(a, v) _Generic((a), \
    mat2: mat2_vec_mult,                \
    mat3: mat3_vec_mult,                \
    mat4: mat4_vec_mult,                \
)(a, v)

static inline vec2 mat2_vec_mult(mat2 m, vec2 v) {
    return (vec2){
        .x = m.m[0][0]*v.x + m.m[0][1]*v.y,
        .y = m.m[1][0]*v.x + m.m[1][1]*v.y
    };
}

static inline vec3 mat3_vec_mult(mat3 m, vec3 v) {
    return (vec3){
        .x = m.m[0][0]*v.x + m.m[0][1]*v.y + m.m[0][2]*v.z,
        .y = m.m[1][0]*v.x + m.m[1][1]*v.y + m.m[1][2]*v.z,
        .z = m.m[2][0]*v.x + m.m[2][1]*v.y + m.m[2][2]*v.z
    };
}

static inline vec4 mat4_vec_mult(mat4 m, vec4 v) {
    return (vec4){
        .x = m.m[0][0]*v.x + m.m[0][1]*v.y + m.m[0][2]*v.z + m.m[0][3]*v.w,
        .y = m.m[1][0]*v.x + m.m[1][1]*v.y + m.m[1][2]*v.z + m.m[1][3]*v.w,
        .z = m.m[2][0]*v.x + m.m[2][1]*v.y + m.m[2][2]*v.z + m.m[2][3]*v.w,
        .w = m.m[3][0]*v.x + m.m[3][1]*v.y + m.m[3][2]*v.z + m.m[3][3]*v.w
    };
}

// @vec_dot
#define vec_dot(a, b) _Generic((a), \
    vec2: vec2_dot,                 \
    vec3: vec3_dot,                 \
    vec4: vec4_dot,                 \
)(a, b)

static inline float vec2_dot(vec2 a, vec2 b) {
    return a.x*b.x + a.y*b.y;
}

static inline float vec3_dot(vec3 a, vec3 b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

static inline float vec4_dot(vec4 a, vec4 b) {
    return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}

// @vec_length
#define vec_length(v) _Generic((a), \
    vec2: vec2_length,              \
    vec3: vec3_length,              \
    vec4: vec4_length,              \
)(a)

static inline float vec2_length_sq(vec2 v) {
    return vec2_dot(v,v);
}

static inline float vec2_length(vec2 v) {
    return sqrtf(vec2_length_sq(v));
}

static inline float vec3_length_sq(vec3 v) {
    return vec3_dot(v,v);
}

static inline float vec3_length(vec3 v) {
    return sqrtf(vec3_length_sq(v));
}

static inline float vec4_length_sq(vec4 v) {
    return vec4_dot(v,v);
}

static inline float vec4_length(vec4 v) {
    return sqrtf(vec4_length_sq(v));
}

#define vec_normalize(v) _Generic((a), \
    vec2: vec2_normalize,              \
    vec3: vec3_normalize,              \
    vec4: vec4_normalize,              \
)(a)

static inline vec2 vec2_normalize(vec2 v) {
    float len = vec2_length(v); 
    if (len == 0.0f) return (vec2){0,0};
    return (vec2){ v.x/len, v.y/len };
}

static inline vec3 vec3_normalize(vec3 v) {
    float len = vec3_length(v);
    if (len == 0.0f) return (vec3){0,0,0};
    return (vec3){ v.x/len, v.y/len, v.z/len };
}

static inline vec4 vec4_normalize(vec4 v) {
    float len = vec4_length(v);
    if (len == 0.0f) return (vec4){0,0,0,0};
    return (vec4){ v.x/len, v.y/len, v.z/len, v.w/len };
}

#define vec_distance(a, b) _Generic((a), \
    vec2: vec2_distance,                 \
    vec3: vec3_distance,                 \
    vec4: vec4_distance,                 \
)(a, b)

static inline float vec2_distance(vec2 a, vec2 b) {
    return vec2_length(vec2_sum(a, vec_negate(b)));
}

static inline float vec3_distance(vec3 a, vec3 b) {
    return vec3_length(vec3_sum(a, vec_negate(b)));
}

static inline float vec4_distance(vec4 a, vec4 b) {
    return vec4_length(vec4_sum(a, vec_negate(b)));
}

static inline vec3 vec_cross(vec3 a, vec3 b) {
    return (vec3){
        .x = a.y*b.z - a.z*b.y,
        .y = a.z*b.x - a.x*b.z,
        .z = a.x*b.y - a.y*b.x
    };
}

#define vec_mult(a, b) _Generic((a), \
    vec2: vec2_mult,                 \
    vec3: vec3_mult,                 \
    vec4: vec4_mult,                 \
)(a, b)

static inline vec2 vec2_mult(vec2 a, vec2 b) {
    return (vec2){ a.x*b.x, a.y*b.y };
}

static inline vec3 vec3_mult(vec3 a, vec3 b) {
    return (vec3){ a.x*b.x, a.y*b.y, a.z*b.z };
}

static inline vec4 vec4_mult(vec4 a, vec4 b) {
    return (vec4){ a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w };
}

#define vec_min(a, b) _Generic((a), \
    vec2: vec2_min,                 \
    vec3: vec3_min,                 \
    vec4: vec4_min,                 \
)(a, b)

static inline vec2 vec2_min(vec2 a, vec2 b) {
    return (vec2){
        .x = fminf(a.x,b.x),
        .y = fminf(a.y,b.y),
    };
}

static inline vec3 vec3_min(vec3 a, vec3 b) {
    return (vec3){
        .x = fminf(a.x,b.x),
        .y = fminf(a.y,b.y),
        .z = fminf(a.z,b.z)
    };
}

static inline vec4 vec4_min(vec4 a, vec4 b) {
    return (vec4){
        .x = fminf(a.x,b.x),
        .y = fminf(a.y,b.y),
        .z = fminf(a.z,b.z),
        .w = fminf(a.w,b.w)
    };
}

#define vec_max(a, b) _Generic((a), \
    vec2: vec2_max,                 \
    vec3: vec3_max,                 \
    vec4: vec4_max,                 \
)(a, b)


static inline vec2 vec2_max(vec2 a, vec2 b) {
    return (vec2){
        .x = fmaxf(a.x,b.x),
        .y = fmaxf(a.y,b.y),
    };
}

static inline vec3 vec3_max(vec3 a, vec3 b) {
    return (vec3){
        .x = fmaxf(a.x,b.x),
        .y = fmaxf(a.y,b.y),
        .z = fmaxf(a.z,b.z)
    };
}

static inline vec4 vec4_max(vec4 a, vec4 b) {
    return (vec4){
        .x = fmaxf(a.x,b.x),
        .y = fmaxf(a.y,b.y),
        .z = fmaxf(a.z,b.z),
        .w = fmaxf(a.w,b.w)
    };
}

#define vec_lerp(a, b, t) _Generic((a), \
    vec2: vec2_lerp,                    \
    vec3: vec3_lerp,                    \
    vec4: vec4_lerp,                    \
)(a, b, t)

static inline vec2 vec2_lerp(vec2 a, vec2 b, float t) {
    return (vec2){
        .x = a.x + (b.x - a.x)*t,
        .y = a.y + (b.y - a.y)*t,
    };
}

static inline vec3 vec3_lerp(vec3 a, vec3 b, float t) {
    return (vec3){
        .x = a.x + (b.x - a.x)*t,
        .y = a.y + (b.y - a.y)*t,
        .z = a.z + (b.z - a.z)*t
    };
}

static inline vec4 vec4_lerp(vec4 a, vec4 b, float t) {
    return (vec4){
        .x = a.x + (b.x - a.x)*t,
        .y = a.y + (b.y - a.y)*t,
        .z = a.z + (b.z - a.z)*t,
        .w = a.w + (b.w - a.w)*t

    };
}

#define vec_clamp(v, lo, hi) _Generic((v), \
    vec2: vec2_clamp,                      \
    vec3: vec3_clamp,                      \
    vec4: vec4_clamp,                      \
)(v, lo, hi)

static inline vec2 vec2_clamp(vec2 v, vec2 lo, vec2 hi) {
    return (vec2){
        .x = fminf(fmaxf(v.x, lo.x), hi.x),
        .y = fminf(fmaxf(v.y, lo.y), hi.y),
    };
}

static inline vec3 vec3_clamp(vec3 v, vec3 lo, vec3 hi) {
    return (vec3){
        .x = fminf(fmaxf(v.x, lo.x), hi.x),
        .y = fminf(fmaxf(v.y, lo.y), hi.y),
        .z = fminf(fmaxf(v.z, lo.z), hi.z)
    };
}

static inline vec4 vec4_clamp(vec4 v, vec4 lo, vec4 hi) {
    return (vec4){
        .x = fminf(fmaxf(v.x, lo.x), hi.x),
        .y = fminf(fmaxf(v.y, lo.y), hi.y),
        .z = fminf(fmaxf(v.z, lo.z), hi.z),
        .w = fminf(fmaxf(v.w, lo.w), hi.w)
    };
}

static inline vec3 vec3_saturate(vec3 v) {
    // clamp each component to [0,1]
    return (vec3){
        .x = fminf(fmaxf(v.x, 0.0f), 1.0f),
        .y = fminf(fmaxf(v.y, 0.0f), 1.0f),
        .z = fminf(fmaxf(v.z, 0.0f), 1.0f)
    };
}

static inline vec3 vec3_reflect(vec3 I, vec3 N) {
    // I - 2*dot(I,N)*N
    float d = vec3_dot(I,N);
    return (vec3){
        .x = I.x - 2.0f*d*N.x,
        .y = I.y - 2.0f*d*N.y,
        .z = I.z - 2.0f*d*N.z
    };
}

static inline vec3 vec3_refract(vec3 I, vec3 N, float eta) {
    float cosi = -fminf(fmaxf(vec3_dot(I,N), -1.0f), 1.0f);
    float k = 1.0f - eta*eta*(1.0f - cosi*cosi);
    if (k < 0.0f) return (vec3){0,0,0};
    float a = eta*cosi - sqrtf(k);
    return (vec3){
        .x = eta*I.x + a*N.x,
        .y = eta*I.y + a*N.y,
        .z = eta*I.z + a*N.z
    };
}

static inline mat4 mat4_identity(void) {
    return (mat4){ .m = {
        {1,0,0,0},
        {0,1,0,0},
        {0,0,1,0},
        {0,0,0,1}
    }};
}

static inline mat4 mat4_transpose(mat4 m) {
    return (mat4){ .m = {
        { m.m[0][0], m.m[1][0], m.m[2][0], m.m[3][0] },
        { m.m[0][1], m.m[1][1], m.m[2][1], m.m[3][1] },
        { m.m[0][2], m.m[1][2], m.m[2][2], m.m[3][2] },
        { m.m[0][3], m.m[1][3], m.m[2][3], m.m[3][3] }
    }};
}

static inline float mat4_determinant(mat4 m) {
    #define M(i,j) m.m[i][j]
    float det =
        M(0,0) * (
            M(1,1)*(M(2,2)*M(3,3) - M(2,3)*M(3,2)) -
            M(1,2)*(M(2,1)*M(3,3) - M(2,3)*M(3,1)) +
            M(1,3)*(M(2,1)*M(3,2) - M(2,2)*M(3,1))
        )
      - M(0,1) * (
            M(1,0)*(M(2,2)*M(3,3) - M(2,3)*M(3,2)) -
            M(1,2)*(M(2,0)*M(3,3) - M(2,3)*M(3,0)) +
            M(1,3)*(M(2,0)*M(3,2) - M(2,2)*M(3,0))
        )
      + M(0,2) * (
            M(1,0)*(M(2,1)*M(3,3) - M(2,3)*M(3,1)) -
            M(1,1)*(M(2,0)*M(3,3) - M(2,3)*M(3,0)) +
            M(1,3)*(M(2,0)*M(3,1) - M(2,1)*M(3,0))
        )
      - M(0,3) * (
            M(1,0)*(M(2,1)*M(3,2) - M(2,2)*M(3,1)) -
            M(1,1)*(M(2,0)*M(3,2) - M(2,2)*M(3,0)) +
            M(1,2)*(M(2,0)*M(3,1) - M(2,1)*M(3,0))
        );
    #undef M
    return det;
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

static inline mat4 mat4_scale_uniform(mat4 m, float s) {
    mat4 out = m;
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            out.m[i][j] *= s;
    return out;
}

static inline mat4 mat4_translate(float x, float y, float z) {
    mat4 o = mat4_identity();
    o.m[0][3] = x;
    o.m[1][3] = y;
    o.m[2][3] = z;
    return o;
}

static inline mat4 mat4_scale(float x, float y, float z) {
    mat4 o = mat4_identity();
    o.m[0][0] = x;
    o.m[1][1] = y;
    o.m[2][2] = z;
    return o;
}

static inline mat4 mat4_rotate_x(float r) {
    float c = cosf(r), s = sinf(r);
    mat4 o = mat4_identity();
    o.m[1][1] =  c; o.m[1][2] = -s;
    o.m[2][1] =  s; o.m[2][2] =  c;
    return o;
}

static inline mat4 mat4_rotate_y(float r) {
    float c = cosf(r), s = sinf(r);
    mat4 o = mat4_identity();
    o.m[0][0] =  c; o.m[0][2] =  s;
    o.m[2][0] = -s; o.m[2][2] =  c;
    return o;
}

static inline mat4 mat4_rotate_z(float r) {
    float c = cosf(r), s = sinf(r);
    mat4 o = mat4_identity();
    o.m[0][0] =  c; o.m[0][1] = -s;
    o.m[1][0] =  s; o.m[1][1] =  c;
    return o;
}

static inline mat4 mat4_perspective(float fovY, float aspect,
                                    float znear, float zfar) {
    float f = 1.0f / tanf(fovY * 0.5f);
    mat4 o = { 0 };
    o.m[0][0] = f / aspect;
    o.m[1][1] = f;
    o.m[2][2] = (zfar + znear) / (znear - zfar);
    o.m[2][3] = (2 * zfar * znear) / (znear - zfar);
    o.m[3][2] = -1.0f;
    return o;
}

static inline mat4 mat4_orthographic(float left, float right,
                                     float bottom, float top,
                                     float znear, float zfar) {
    mat4 o = {0};
    o.m[0][0] =  2.0f / (right - left);
    o.m[1][1] =  2.0f / (top   - bottom);
    o.m[2][2] = -2.0f / (zfar  - znear);
    o.m[0][3] = -(right + left)   / (right - left);
    o.m[1][3] = -(top   + bottom) / (top   - bottom);
    o.m[2][3] = -(zfar  + znear)  / (zfar  - znear);
    o.m[3][3] =  1.0f;
    return o;
}

static inline mat4 mat4_look_at(vec3 eye, vec3 center, vec3 up) {
    vec3 f = vec3_normalize(vec3_sub(center, eye));
    vec3 s = vec3_normalize(vec3_cross(f, up));
    vec3 u = vec3_cross(s, f);
    mat4 o = mat4_identity();
    o.m[0][0]= s.x; o.m[0][1]= s.y; o.m[0][2]= s.z;
    o.m[1][0]= u.x; o.m[1][1]= u.y; o.m[1][2]= u.z;
    o.m[2][0]=-f.x; o.m[2][1]=-f.y; o.m[2][2]=-f.z;
    o.m[0][3] = -vec3_dot(s, eye);
    o.m[1][3] = -vec3_dot(u, eye);
    o.m[2][3] =  vec3_dot(f, eye);
    return o;
}

#endif
