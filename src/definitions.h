#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <box2d/box2d.h>


// These are shared between files, and are defined in definitions.c
const int WINDOW_WIDTH;
const int WINDOW_HEIGHT;
const float SCALE;
const int NUMBER_OF_POINTS;

const float HALF_BOX_WIDTH;
const float HALF_BOX_HEIGHT;

const float HALF_BOX_WIDTH;
const float HALF_BOX_HEIGHT;

const int numberOfShips;


const int NUMBER_OF_POINTS;
extern b2Vec2 shipPoints[];



typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} Color;

// Struct for holding PID controller data
typedef struct {
    float Kp;
    float Ki;
    float Kd;
    float integral;
    float previousError;
} PIDController;


typedef struct {
    b2BodyId bodyId;

    b2Vec2 points[7]; // Hull points
    
    float width;
    float height;

    // Motor/thruster data
    b2Vec2 leftMotorPosition;  // Position of the left motor
    b2Vec2 rightMotorPosition; // Position of the right motor
    float leftMotorForce;      // Force applied by the left motor
    float rightMotorForce;     // Force applied by the right motor

    b2Vec2 target;            // Target position
    b2Rot targetOrientation;  // Target orientation

    Color color;              // Ship's color

    PIDController pidController; // PID controller for the ship
} Ship;



// vector functionsb2Vec2 subtract(b2Vec2 a, b2Vec2 b) {
b2Vec2 subtract(b2Vec2 a, b2Vec2 b);

b2Vec2 scale(b2Vec2 v, float scalar);

b2Vec2 add(b2Vec2 a, b2Vec2 b);

b2Vec2 rotate(b2Vec2 v, b2Rot rotation);

#endif // DEFINITIONS_H