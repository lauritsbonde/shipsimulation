#include <box2d/box2d.h>
#include </opt/homebrew/include/SDL2/SDL.h>
#include <stdio.h>

#include "src/definitions.h"
#include "src/render.h"
#include "src/ships.h"
#include "src/formationCalculator.h"

void createBoundary(b2WorldId worldId, float halfWidth, float halfHeight) {
    // Define common properties for wall shapes
    b2ShapeDef wallShapeDef = b2DefaultShapeDef();
    wallShapeDef.density = 0.0f;  // Static walls should have zero density

    // Left wall
    b2BodyDef leftWallDef = b2DefaultBodyDef();
    leftWallDef.position = (b2Vec2){-halfWidth, 0.0f};
    b2BodyId leftWallBodyId = b2CreateBody(worldId, &leftWallDef);
    b2Polygon leftWallBox = b2MakeBox(0.5f, halfHeight);
    b2CreatePolygonShape(leftWallBodyId, &wallShapeDef, &leftWallBox);

    // Right wall
    b2BodyDef rightWallDef = b2DefaultBodyDef();
    rightWallDef.position = (b2Vec2){halfWidth, 0.0f};
    b2BodyId rightWallBodyId = b2CreateBody(worldId, &rightWallDef);
    b2Polygon rightWallBox = b2MakeBox(0.5f, halfHeight);
    b2CreatePolygonShape(rightWallBodyId, &wallShapeDef, &rightWallBox);

    // Bottom wall
    b2BodyDef bottomWallDef = b2DefaultBodyDef();
    bottomWallDef.position = (b2Vec2){0.0f, -halfHeight};
    b2BodyId bottomWallBodyId = b2CreateBody(worldId, &bottomWallDef);
    b2Polygon bottomWallBox = b2MakeBox(halfWidth, 0.5f);
    b2CreatePolygonShape(bottomWallBodyId, &wallShapeDef, &bottomWallBox);

    // Top wall
    b2BodyDef topWallDef = b2DefaultBodyDef();
    topWallDef.position = (b2Vec2){0.0f, halfHeight};
    b2BodyId topWallBodyId = b2CreateBody(worldId, &topWallDef);
    b2Polygon topWallBox = b2MakeBox(halfWidth, 0.5f);
    b2CreatePolygonShape(topWallBodyId, &wallShapeDef, &topWallBox);
}


int main() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Window *window = SDL_CreateWindow("Box2D Simulation", 2000, 2000, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        SDL_DestroyWindow(window);
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create Box2D world
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, 0.0f};
    b2WorldId worldId = b2CreateWorld(&worldDef);;

    // Create boundary walls around the world
    createBoundary(worldId, HALF_BOX_WIDTH, HALF_BOX_HEIGHT);

    // Create dynamic body
    Ship ships[numberOfShips];  // Array to store multiple ships
    createShips(worldId, numberOfShips, ships);

    // calculate the formation
    Formation* formation = createFormation(numberOfShips);
    Shape* shape = createShape(shipPoints, NUMBER_OF_POINTS);
    calculateFormation(formation, shape);

    for (int i = 0; i < numberOfShips; i++) {
        ships[i].target = formation->targets[i];
    }

    // Simulation parameters
    float timeStep = 1.0f / 60.0f;
    // lover subStepCount means better performance but less accuracy, and vice versa, 4 is recommended
    int subStepCount = 4;

    int running = 1;
    SDL_Event e;

    // Main loop
    while (running) {
        // Event handling
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
        }

        // Step the physics world
        b2World_Step(worldId, timeStep, subStepCount);

        // Render - Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        // Render boundary walls
        Color boundaryColor = {255, 0, 0, 255};
        render_box(renderer, (b2Vec2){0.0f, 0.0f}, (b2Rot){1.0f, 0.0f}, HALF_BOX_WIDTH, HALF_BOX_HEIGHT, boundaryColor);

        // Move boxes to target
        moveShipsToTarget(numberOfShips, ships);

        // render ships
        renderShips(numberOfShips, ships, renderer);

        SDL_RenderPresent(renderer);

        SDL_Delay(1000 / 60); // Cap frame rate
    }

    // Cleanup
    b2DestroyWorld(worldId);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
