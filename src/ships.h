#ifndef SHIPS_H
#define SHIPS_H

#include <box2d/box2d.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "definitions.h"

void createShips(b2WorldId worldId, int numberOfShips, Ship* ships);
void moveShipsToTarget(int numberOfShips, Ship* ships, float deltaTime);

#endif // SHIPS_H