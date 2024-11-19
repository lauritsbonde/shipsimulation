#ifndef RENDER_H
#define RENDER_H

#include <box2d/box2d.h>
#include </opt/homebrew/include/SDL2/SDL.h>
#include "definitions.h"

void render_box(SDL_Renderer *renderer, b2Vec2 position, b2Rot rotation, float width, float height, Color color);
void renderShips(int numberOfShips, Ship* ships, SDL_Renderer* renderer);

#endif // RENDER_H