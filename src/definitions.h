#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <box2d/box2d.h>

const int WINDOW_WIDTH;
const int WINDOW_HEIGHT;
const float SCALE;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} Color;

typedef struct {
    b2BodyId bodyId;
    
    float width;
    float height;


    float leftMotorForce;
    float rightMotorForce;

    b2Vec2 target;

    Color color;
} Ship;


#endif // DEFINITIONS_H