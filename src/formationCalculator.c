#include "formationCalculator.h"


Shape* createShape(b2Vec2* points, int numberOfPoints) {
    // Allocate memory for the Shape structure and its points array
    Shape* shape = malloc(sizeof(Shape) + numberOfPoints * sizeof(b2Vec2));
    if (!shape) {
        return NULL; // Handle memory allocation failure
    }

    // Set the number of points
    shape->numberOfPoints = numberOfPoints;

    // Copy the points into the flexible array member
    for (int i = 0; i < numberOfPoints; i++) {
        shape->points[i] = points[i];
    }

    return shape;
}

Formation* createFormation(int numberOfShips) {
    // Allocate memory for the Formation structure and its targets array
    Formation* formation = malloc(sizeof(Formation) + numberOfShips * sizeof(b2Vec2));
    if (!formation) {
        return NULL; // Handle memory allocation failure
    }

    // Set the number of ships
    formation->numberOfShips = numberOfShips;

    return formation;
}

// This function returns the target positions for each ship in a formation starting of as a pyramid to a certain point and then a line
#include <math.h>
#include <stdlib.h>

// This function calculates the target positions for each ship in a pyramid-to-line formation
void calculateFormation(Formation* outFormation, Shape* shape) {
    int totalShips = outFormation->numberOfShips;

    int currentShip = 0;
    int level = 0;
    float spacing = 2.3f; // Spacing between ships
    float lastLevelWidth = 0.0f; // Width of the last pyramid level

    // Calculate pyramid formation
    while (currentShip < totalShips) {
        level++;
        int shipsInLevel = level;

        // Determine if the remaining ships fit in this level
        if (currentShip + shipsInLevel > totalShips) {
            break; // Stop pyramid and transition to a line
        }

        // Calculate x-position offset to center-align the level
        float xOffset = -0.5f * (shipsInLevel - 1) * spacing;

        for (int i = 0; i < shipsInLevel; i++) {
            outFormation->targets[currentShip].x = xOffset + i * spacing;
            outFormation->targets[currentShip].y = -level * spacing; // Negative y-axis for downward pyramid
            currentShip++;
        }

        // Update the width of the last level
        lastLevelWidth = (shipsInLevel - 1) * spacing;
    }

    // Calculate the starting x-coordinate for the line
    float lineStartX = 2.0f * lastLevelWidth;

    // Transition to line formation
    for (; currentShip < totalShips; currentShip++) {
        int lineShipIndex = currentShip - (level * (level + 1)) / 2; // Index in line
        outFormation->targets[currentShip].x = lineStartX + lineShipIndex * spacing;
        outFormation->targets[currentShip].y = -(level) * spacing; // Line continues below the pyramid
    }
}


