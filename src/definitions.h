#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <box2d/box2d.h>


// These are shared between files, and are defined in main.c
const int WINDOW_WIDTH;
const int WINDOW_HEIGHT;
const int NUMBER_OF_POINTS; // Number of points in the ship's hull 7
const float SCALE;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} Color;


typedef struct {
    b2BodyId bodyId;

    b2Vec2 points[7];
    
    float width;
    float height;


    float leftMotorForce;
    float rightMotorForce;

    b2Vec2 target;
    b2Rot targetOrientation;

    Color color;
} Ship;


#endif // DEFINITIONS_H