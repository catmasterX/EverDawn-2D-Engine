#ifndef MOVEMENT_AND_COLLISION_H
#define MOVEMENT_AND_COLLISION_H

#include "engine.h"
#include <math.h>
#include <stdlib.h>

// Funciones para movimiento y colisiones
void handlePlayerMovement(Player* player, Platform* platforms, int platformCount, Sprite* sprite_player);
int checkCollision(Player* player, Platform* platform);

#endif