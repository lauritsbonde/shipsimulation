#include "render.h"
#include "definitions.h"

void render_ship(SDL_Renderer *renderer, b2Vec2 position, b2Rot rotation, b2Vec2* points, Color color) {
    // Calculate screen center position
    float centerX = position.x * SCALE + WINDOW_WIDTH / 2;
    float centerY = WINDOW_HEIGHT / 2 - position.y * SCALE;

    // Transform and rotate the ship's points
    SDL_FPoint transformedPoints[NUMBER_OF_POINTS]; // Include closing point for loop
    for (int i = 0; i < NUMBER_OF_POINTS-1; i++) {
        // Local coordinates of the ship point
        float localX = points[i].x * SCALE;
        float localY = points[i].y * SCALE;

        // Rotate around the center of the ship with an additional 90° clockwise rotation
        float rotatedX = localX * rotation.s - localY * rotation.c;
        float rotatedY = localX * rotation.c + localY * rotation.s;

        // Translate to screen coordinates
        transformedPoints[i].x = centerX + rotatedX;
        transformedPoints[i].y = centerY + rotatedY;
    }

    // Close the polygon loop
    transformedPoints[NUMBER_OF_POINTS-1] = transformedPoints[0];

    // Set draw color for the ship
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    // Draw the ship polygon
    SDL_RenderDrawLinesF(renderer, transformedPoints, NUMBER_OF_POINTS);

    // Draw the ship's orientatiozn line
    float orientationLineLength = 2.0f * SCALE;
    SDL_FPoint orientationLine[2] = {
        {centerX, centerY},
        {centerX + rotation.c * orientationLineLength, centerY - rotation.s * orientationLineLength}
    };
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a * 0.95);
    SDL_RenderDrawLinesF(renderer, orientationLine, 2);
}

void render_motors(Ship ship, SDL_Renderer* renderer) {
    b2Vec2 position = b2Body_GetPosition(ship.bodyId);
    b2Rot rotation = b2Body_GetRotation(ship.bodyId);

    // Calculate the position of the left motor
    b2Vec2 leftMotorPosition = add(position, rotate(ship.leftMotorPosition, rotation));
    // Calculate the position of the right motor
    b2Vec2 rightMotorPosition = add(position, rotate(ship.rightMotorPosition, rotation));

    // Convert motor positions to screen coordinates
    float leftMotorX = leftMotorPosition.x * SCALE + WINDOW_WIDTH / 2;
    float leftMotorY = WINDOW_HEIGHT / 2 - leftMotorPosition.y * SCALE;
    float rightMotorX = rightMotorPosition.x * SCALE + WINDOW_WIDTH / 2;
    float rightMotorY = WINDOW_HEIGHT / 2 - rightMotorPosition.y * SCALE;

    // Set draw color for the motors
    SDL_SetRenderDrawColor(renderer, ship.color.r, ship.color.g, ship.color.b, ship.color.a);

    // Draw the motors as small rectangles or points
    SDL_Rect leftMotorRect = {leftMotorX - 2, leftMotorY - 2, 4, 4};
    SDL_Rect rightMotorRect = {rightMotorX - 2, rightMotorY - 2, 4, 4};
    SDL_RenderFillRect(renderer, &leftMotorRect);
    SDL_RenderFillRect(renderer, &rightMotorRect);
}

void render_target(Ship ship, SDL_Renderer* renderer){
    Color color = ship.color;
    color.a = 100;
    render_ship(renderer, ship.target, ship.targetOrientation, ship.points, color);
}

void renderShips(int numberOfShips, Ship* ships, SDL_Renderer* renderer) {
    for (int i = 0; i < numberOfShips; i++) {
        b2Vec2 position = b2Body_GetPosition(ships[i].bodyId);
        b2Rot rotation = b2Body_GetRotation(ships[i].bodyId);
        render_ship(renderer, position, rotation, ships[i].points, ships[i].color);
        render_motors(ships[i], renderer);
        render_target(ships[i], renderer);
    }
}

void render_box(SDL_Renderer *renderer, b2Vec2 position, b2Rot rotation, float width, float height, Color color) {
    // Calculate the half-width and half-height
    float halfWidth = width * SCALE;
    float halfHeight = height * SCALE;

    // Calculate the center position on the screen, scaling and flipping y-axis as needed
    float centerX = position.x * SCALE + WINDOW_WIDTH / 2;
    float centerY = WINDOW_HEIGHT / 2 - position.y * SCALE;

    // Calculate the rotated positions of the rectangle's corners
    float cosAngle = rotation.c;
    float sinAngle = rotation.s;

    // Top-left corner
    float x1 = centerX - halfWidth * cosAngle + halfHeight * sinAngle;
    float y1 = centerY - halfWidth * sinAngle - halfHeight * cosAngle;

    // Top-right corner
    float x2 = centerX + halfWidth * cosAngle + halfHeight * sinAngle;
    float y2 = centerY + halfWidth * sinAngle - halfHeight * cosAngle;

    // Bottom-right corner
    float x3 = centerX + halfWidth * cosAngle - halfHeight * sinAngle;
    float y3 = centerY + halfWidth * sinAngle + halfHeight * cosAngle;

    // Bottom-left corner
    float x4 = centerX - halfWidth * cosAngle - halfHeight * sinAngle;
    float y4 = centerY - halfWidth * sinAngle + halfHeight * cosAngle;

    // Set draw color to white
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    // Draw the rotated rectangle using lines between the corners
    SDL_RenderDrawLineF(renderer, x1, y1, x2, y2);
    SDL_RenderDrawLineF(renderer, x2, y2, x3, y3);
    SDL_RenderDrawLineF(renderer, x3, y3, x4, y4);
    SDL_RenderDrawLineF(renderer, x4, y4, x1, y1);
}