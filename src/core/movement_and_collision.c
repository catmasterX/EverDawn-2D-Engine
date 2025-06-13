#include "movement_and_collision.h"
#include <math.h>

#define EPSILON                        0.001f
#define GRAVITY                        1
#define JUMP_SPEED                    -10
#define MAX_WALK_SPEED                 6
#define WALK_ACCELERATION              0.5f
#define MAX_RUN_SPEED                  8
#define RUN_ACCELERATION               0.1f    // Incremento gradual para correr
#define JUMP_HOLD_LIMIT                0.2f    // Segundos salto variable
#define WALK_ANIMATION_SPEED_DEFAULT 110      // ms por cuadro caminata
#define WALK_ANIMATION_SPEED_RUN      90       // ms por cuadro carrera

// Parámetros “mario stop”
#define SLIDE_THRESHOLD 0.1f
#define LERP_FACTOR     0.2f

// LERP lineal
static float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

void updatePlayerPosition(Player* player, float desiredSpeedX,
                          int platformCount, Platform* platforms)
{
    // --- Movimiento horizontal + colisión ---
    player->x += (int)desiredSpeedX;
    for (int i = 0; i < platformCount; i++) {
        if (checkCollision(player, &platforms[i])) {
            if (desiredSpeedX > 0)
                player->x = platforms[i].x - player->currentSprite->width  - EPSILON;
            else if (desiredSpeedX < 0)
                player->x = platforms[i].x + platforms[i].width + EPSILON;
            break;
        }
    }

    // --- Movimiento vertical + colisión ---
    player->y += player->speedY;
    bool onGround = false;
    for (int i = 0; i < platformCount; i++) {
        if (checkCollision(player, &platforms[i])) {
            if (player->speedY > 0 &&
               player->y + player->currentSprite->height - player->speedY
               <= platforms[i].y + EPSILON)
            {
                player->y      = platforms[i].y - player->currentSprite->height;
                player->speedY = 0;
                player->state  = STANDING;
                onGround       = true;
            }
            else if (player->speedY < 0) {
                player->y      = platforms[i].y + platforms[i].height + EPSILON;
                player->speedY = 0;
            }
            break;
        }
    }
    if (!onGround) {
        player->state = FALLING;
    }
}

int checkCollision(Player* player, Platform* platform) {
    bool collisionX = player->x <  platform->x + platform->width  &&
                      player->x + player->currentSprite->width > platform->x;
    bool collisionY = player->y <  platform->y + platform->height &&
                      player->y + player->currentSprite->height> platform->y;
    return collisionX && collisionY;
}

bool isOnTopOfPlatform(Player* player, Platform* platform) {
    return player->y   + player->currentSprite->height <= platform->y + EPSILON &&
           player->y   + player->currentSprite->height >= platform->y - EPSILON &&
           player->x   + player->currentSprite->width  > platform->x &&
           player->x   <  platform->x + platform->width;
}

void handlePlayerMovement(Player* player,
                          Platform* platforms,
                          int platformCount,
                          Sprite* sprite_player)
{
    const Uint8* keyState   = SDL_GetKeyboardState(NULL);
    Uint32      currentTime = SDL_GetTicks();

    bool pressD = keyState[SDL_SCANCODE_D];
    bool pressA = keyState[SDL_SCANCODE_A];
    bool pressK = keyState[SDL_SCANCODE_K];
    bool pressJ = keyState[SDL_SCANCODE_J];

    // ¿En suelo?
    bool onGround = false;
    for (int i = 0; i < platformCount; i++) {
        if (isOnTopOfPlatform(player, &platforms[i])) {
            onGround = true;
            break;
        }
    }

    // Variables estáticas (persisten entre frames)
    static float  currentSpeedX      = 0.0f;  // magnitud de velocidad horizontal
    static int    lastInputDirection = 1;     //  1=derecha, -1=izquierda
    static bool   isSliding          = false; // fase de “mario stop”
    static Uint32 lastWalkUpdate     = 0;
    static Uint32 jumpStartTime      = 0;
    static int    jumpKeyHeld        = 0;
    static int    jumpFacing         = 1;     // Dirección al inicio del salto

    float desiredSpeedX = 0.0f;

    // --- MOVIMIENTO HORIZONTAL (“mario stop” sólo al correr) ---
    if (pressD || pressA) {
        int  inputDir = pressD ? 1 : -1;
        bool running  = pressK && (currentSpeedX >= MAX_WALK_SPEED);

        if (isSliding && onGround) {
            // Continuar deslizando (solo si está en el suelo)
            currentSpeedX = lerp(currentSpeedX, 0.0f, LERP_FACTOR);
            if (currentSpeedX < SLIDE_THRESHOLD) {
                currentSpeedX      = 0.0f;
                isSliding          = false;
                lastInputDirection = inputDir;
            }
        }
        else if (running && inputDir != lastInputDirection && onGround) {
            // Inicio de slide (sólo si corre y gira bruscamente en el suelo)
            isSliding = true;
        }
        else {
            // Aceleración normal (o giro instantáneo si no corre)
            isSliding          = false;
            lastInputDirection = inputDir;
            if (running) {
                currentSpeedX += RUN_ACCELERATION;
                if (currentSpeedX > MAX_RUN_SPEED) currentSpeedX = MAX_RUN_SPEED;
            } else {
                currentSpeedX += WALK_ACCELERATION;
                if (currentSpeedX > MAX_WALK_SPEED) currentSpeedX = MAX_WALK_SPEED;
            }
        }

        desiredSpeedX = currentSpeedX * lastInputDirection;
    }
    else {
        // Sin input horizontal: frenar
        isSliding     = false;
        currentSpeedX = 0.0f;
        desiredSpeedX = 0.0f;
    }

    // --- SALTO VARIABLE + GRAVEDAD ---
    if (pressJ) {
        if (onGround && !jumpKeyHeld) {
            player->speedY   = JUMP_SPEED;
            jumpStartTime    = currentTime;
            jumpKeyHeld      = 1;
            player->state    = JUMPING;
            jumpFacing       = lastInputDirection; // Guardar dirección al inicio del salto
        }
        else if (jumpKeyHeld &&
                 (currentTime - jumpStartTime) < JUMP_HOLD_LIMIT * 1000 &&
                 player->speedY < 0)
        {
            player->speedY = JUMP_SPEED -
                             (pressK && currentSpeedX >= MAX_RUN_SPEED ? 2 : 0);
        }
    } else {
        jumpKeyHeld = 0;
    }
    player->speedY += GRAVITY;
    if (player->state == JUMPING) {
        player->currentSprite = &sprite_player[5];  // jump
    }

    // --- SPRITE Y ESTADO EN SUELO ---
    if (onGround && player->state != JUMPING) {
        if (isSliding) {
            player->state         = STOPPING;
            player->currentSprite = &sprite_player[4];
        }
        else if (fabs(desiredSpeedX) > 0.0f) {
            bool running = pressK && (currentSpeedX >= MAX_WALK_SPEED);
            player->state = running ? RUNNING : WALKING;
            Uint32 animSpd = running
                              ? WALK_ANIMATION_SPEED_RUN
                              : WALK_ANIMATION_SPEED_DEFAULT;
            if (currentTime - lastWalkUpdate > animSpd) {
                player->currentSprite =
                  &sprite_player[1 + ((player->currentSprite - &sprite_player[1] + 1) % 3)];
                lastWalkUpdate = currentTime;
            }
        }
        else {
            player->state         = STANDING;
            player->currentSprite = &sprite_player[0];
        }
    }

    // --- FLIP DEL SPRITE ---
    if (player->state == JUMPING || player->state == FALLING) {
        // Mantener el flip al saltar basado en la dirección inicial del salto
        player->currentSprite->flip =
          (jumpFacing == 1) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    } else if (player->state == STOPPING) {
        // Al hacer skid, el sprite viene orientado al revés, invertimos el flip
        player->currentSprite->flip =
          (lastInputDirection == 1) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    } else {
        // Flip normal para otros estados
        player->currentSprite->flip =
          (lastInputDirection == 1) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    }

    // --- APLICAR MOVIMIENTO Y COLISIONES ---
    updatePlayerPosition(player, desiredSpeedX, platformCount, platforms);
}