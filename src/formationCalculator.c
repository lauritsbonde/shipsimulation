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

// This function calculates the maximum level of a pyramid given the number of ships
int calculateLevel(int numberOfShips) {
    return (int)((-1 + sqrt(1 + 8 * numberOfShips)) / 2);
}

// This function calculates the number of blocks needed to build a pyramid of a given level
int calculateBlocksNeeded(int level) {
    return level * (level + 1) / 2;
}

// This function calculates the target positions for each ship in a pyramid-to-line formation
void calculateFormation(Formation* outFormation, Shape* shape) {
    int totalShips = outFormation->numberOfShips;

    int currentShip = 0;
    int pyramidLevel = 0;
    float spacing = 1.25f; // Spacing between ships
    float lastLevelWidth = 0.0f; // Width of the last pyramid level
    int lastLevelBlockWidth = 0;

    // The number of ships in a pyramid shape is the logarithm base 2 of the total number of ships
    int pyramidShips = log2(totalShips);
    // Calculate the blocks needed for the pyramid
    int maxBlocks = calculateBlocksNeeded(pyramidShips);


    // Calculate pyramid formation
    while (currentShip < totalShips) {
        int shipsInLevel = pyramidLevel + 1;

        if(currentShip > maxBlocks){
            break;
        }

        // Determine if the remaining ships fit in this level
        if (currentShip > totalShips) {
            printf("currentShip: %d - totalShips: %d\n", currentShip, totalShips);
            break; // Stop pyramid and transition to a line
        }

        // Calculate x-position offset to center-align the level
        float xOffset = -0.5f * (shipsInLevel - 1) * spacing;

        for (int i = 0; i < shipsInLevel; i++) {
            outFormation->targets[currentShip].x = xOffset + i * spacing;
            outFormation->targets[currentShip].y = -pyramidLevel * spacing; // Negative y-axis for downward pyramid
            currentShip++;
        }

        // Update the width of the last level
        lastLevelWidth = (shipsInLevel - 1) * spacing;
        lastLevelBlockWidth = shipsInLevel;
        pyramidLevel++;
    }

    // Calculate the starting x-coordinate for the line
    float lineStartX = lastLevelWidth * -0.5f;

    int rowLevel = pyramidLevel;

    // create the rows that follows the pyramid formatiuon, the width should be the same as the last level of the pyramid
    while (currentShip < totalShips) {
        int shipsInLevel = lastLevelBlockWidth;

        if((rowLevel - pyramidLevel) % 2 == 0){
            shipsInLevel--;
        }

        printf("row-pyramidLevel: %d\n", rowLevel - pyramidLevel);
        
        for(int i = 0; i < shipsInLevel && currentShip < totalShips; i++){
            float xShift = shipsInLevel < lastLevelBlockWidth ? 0.6f : 0;
            printf("xShift: %f\n", xShift);
            outFormation->targets[currentShip].x = (lineStartX + xShift) + i * spacing;
            outFormation->targets[currentShip].y = -rowLevel * spacing;
            currentShip++;
        }
        rowLevel++;
    }
}


