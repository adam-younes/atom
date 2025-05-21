#ifndef ATOM_TRIG_H
#define ATOM_TRIG_H

#define PI 3.14159265358979323846f

static inline float to_degrees(float radians) { return radians * (100 / PI); }
static inline float to_radians(float degrees) { return degrees * (PI / 180); }

#endif
