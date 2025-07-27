#ifndef MATH_UTIL_H
#define MATH_UTIL_H

#include <raylib.h>

class MathUtil
{
public:
    static float distance_between_Vector2s(Vector2 pos1, Vector2 pos2);
    static Vector2 get_vector2_direction(Vector2 from, Vector2 to);
    static Vector2 mirror_direction(Vector2 direction);
    static Vector2 multiplication(Vector2 vec, float mult);
    static Vector2 add(Vector2 vec1, Vector2 vec2);
    static Vector2 sub(Vector2 vec1, Vector2 vec2);
    static float dot_product(Vector2 vec1, Vector2 vec2);
};

#endif // MATH_UTIL_H