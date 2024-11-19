#ifndef SHIPS_H
#define SHIPS_H

#include <box2d/box2d.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "definitions.h"

Ship createShip(b2WorldId worldId, b2Vec2 position, b2Vec2 target);
void createShips(b2WorldId worldId, int numberOfShips, Ship* ships);
void moveShipsToTarget(int numberOfShips, Ship* ships);

void moveShipsToTargetPID(int numberOfShips, Ship* ships, b2Vec2 target);

#endif // SHIPS_H