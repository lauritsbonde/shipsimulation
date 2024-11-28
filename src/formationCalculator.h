#ifndef FORMATIONCALCULATOR_H
#define FORMATIONCALCULATOR_H

#include <box2d/box2d.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "definitions.h"

typedef struct {
  int numberOfPoints;
  b2Vec2 points[];
} Shape;

typedef struct {
  int numberOfShips;
  b2Vec2 targets[];
} Formation;


Shape* createShape(b2Vec2* points, int numberOfPoints);
Formation* createFormation(int numberOfShips);
// This function should return the target positions for each ship in the formation
void calculateFormation(Formation* outFormation, Shape* shape);

#endif // FORMATIONCALCULATOR_H