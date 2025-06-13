#include "engine.h"

void renderSprite(SDL_Renderer* renderer, SDL_Texture* texture, Sprite* sprite, int x, int y, int cameraX, int cameraY) {
    SDL_Rect srcRect = { sprite->sheetX, sprite->sheetY, sprite->width, sprite->height };
    SDL_Rect dstRect = { x - cameraX, y - cameraY, sprite->width, sprite->height };
    SDL_RenderCopyEx(renderer, texture, &srcRect, &dstRect, 0, NULL, sprite->flip);
}

void renderPlatform(SDL_Renderer* renderer, SDL_Texture* texture, Platform* platform, int cameraX, int cameraY) {
    SDL_Rect srcRect = { platform->sprite->sheetX, platform->sprite->sheetY, platform->sprite->width, platform->sprite->height };
    SDL_Rect dstRect = { platform->x - cameraX, platform->y - cameraY, platform->sprite->width, platform->sprite->height };

    for (int y = 0; y < platform->height; y += platform->sprite->height) {
        for (int x = 0; x < platform->width; x += platform->sprite->width) {
            dstRect.x = platform->x + x - cameraX;
            dstRect.y = platform->y + y - cameraY;
            SDL_RenderCopy(renderer, texture, &srcRect, &dstRect);
        }
    }
}