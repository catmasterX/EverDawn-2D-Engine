#ifndef ENGINE_H
#define ENGINE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_net.h>
#include <stdio.h>
#include <stdbool.h>

// Estructura para representar un sprite
typedef struct {
    int sheetX, sheetY; // Posición en la spritesheet
    int width, height;
    SDL_RendererFlip flip; // Dirección del sprite
} Sprite;

// Estados del jugador
typedef enum {
    STANDING,
    WALKING,
    RUNNING,
    JUMPING,
    FALLING,
    STOPPING,
    DEAD
} PlayerState;

// Estructura para el jugador
typedef struct {
    int x, y; // Posición del jugador
    int speedX, speedY; // Velocidad del jugador
    Sprite* currentSprite; // Sprite actual
    PlayerState state;
    SDL_RendererFlip lastFlip; // Última dirección mirada (flip)
} Player;

// Estructura para las plataformas
typedef struct {
    int x, y;
    int width, height;
    Sprite* sprite;
} Platform;

// Estructura para los enemigos
typedef struct {
    int x, y;
    int speedX, speedY;
    Sprite* currentSprite;
} Enemy;

// Estructura para los items
typedef struct {
    int x, y;
    Sprite* sprite;
} Item;

// Estructura para los escenarios
typedef struct {
    int width, height;
    Platform* platforms;
    Enemy* enemies;
    Item* items;
} Scene;

// Funciones para renderizar
void renderSprite(SDL_Renderer* renderer, SDL_Texture* texture, Sprite* sprite, int x, int y, int cameraX, int cameraY);
void renderPlatform(SDL_Renderer* renderer, SDL_Texture* texture, Platform* platform, int cameraX, int cameraY);

#endif