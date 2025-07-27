#include "MathUtil.h"
#include <cmath>

float MathUtil::distance_between_Vector2s(Vector2 pos1, Vector2 pos2)
{
    float dx = pos2.x - pos1.x;
    float dy = pos2.y - pos1.y;
    return std::sqrt(dx * dx + dy * dy);
}

Vector2 MathUtil::get_vector2_direction(Vector2 from, Vector2 to)
{
    Vector2 delta = { to.x - from.x, to.y - from.y };
    float length = sqrtf(delta.x * delta.x + delta.y * delta.y);
    if (length == 0.0f) return { 0.0f, 0.0f };
    return { delta.x / length, delta.y / length };
}

Vector2 MathUtil::mirror_direction(Vector2 direction)
{
    return { direction.y, direction.x };
}

Vector2 MathUtil::multiplication(Vector2 vec, float mult)
{
    return { vec.x * mult, vec.y * mult };
}

Vector2 MathUtil::add(Vector2 vec1, Vector2 vec2)
{
    return { vec1.x + vec2.x, vec1.y + vec2.y };
}

Vector2 MathUtil::sub(Vector2 vec1, Vector2 vec2)
{
    return { vec1.x - vec2.x, vec1.y - vec2.y };
}

float MathUtil::dot_product(Vector2 vec1, Vector2 vec2)
{
    return (vec1.x * vec2.x + vec1.y * vec2.y);
}
