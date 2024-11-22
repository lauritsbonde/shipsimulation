#include "definitions.h"

const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;
const float SCALE = 20.0f; 
const int NUMBER_OF_POINTS = 7;

const float HALF_BOX_WIDTH = 35.0f;
const float HALF_BOX_HEIGHT = 25.0f;

const int numberOfShips = 5;

b2Vec2 shipPoints[] = {
    {0.0f, 1.0f},   // Top point (elongated)
    {0.5f, 0.3f},   // Top-right corner
    {0.5f, -0.3f},  // Bottom-right corner
    {0.0f, -1.0f},  // Bottom point (elongated)
    {-0.5f, -0.3f}, // Bottom-left corner
    {-0.5f, 0.3f},  // Top-left corner
    {0.0f, 1.0f}    // Close the loop
};


// vector math functions
b2Vec2 subtract(b2Vec2 a, b2Vec2 b) {
    b2Vec2 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    return result;
}

b2Vec2 scale(b2Vec2 v, float scalar) {
    b2Vec2 result;
    result.x = v.x * scalar;
    result.y = v.y * scalar;
    return result;
}

b2Vec2 add(b2Vec2 a, b2Vec2 b) {
    b2Vec2 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    return result;
}

b2Vec2 rotate(b2Vec2 v, float angle) {
    float cosAngle = cosf(angle);
    float sinAngle = sinf(angle);

    return (b2Vec2){
        .x = v.x * cosAngle - v.y * sinAngle,
        .y = v.x * sinAngle + v.y * cosAngle
    };
}
