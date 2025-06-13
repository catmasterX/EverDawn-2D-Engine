#include "engine.h"
#include "movement_and_collision.h"

// Prototipo para dibujar FPS
void drawFPS(SDL_Renderer* renderer, TTF_Font* font, int fps);

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0 || SDLNet_Init() < 0) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return -1;
    }

    if (IMG_Init(IMG_INIT_PNG) == 0 || TTF_Init() == -1 || Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        SDL_Log("Failed to initialize SDL extensions");
        SDL_Quit();
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("EverDawn",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (!window) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    SDL_Texture* spriteSheet = IMG_LoadTexture(renderer, "../assets/sprites/spritesheet.png");
    if (!spriteSheet) {
        SDL_Log("Failed to load texture: %s", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    TTF_Font* font = TTF_OpenFont("../assets/fonts/arial.ttf", 24);
    if (!font) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
        SDL_DestroyTexture(spriteSheet);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    Player player = { 100, 100, 0, 0, STANDING, SDL_FLIP_NONE };
    Sprite sprite_player[] = {
        {0, 0, 36, 36, SDL_FLIP_NONE}, //stand
        {36, 0, 36, 36, SDL_FLIP_NONE}, //Walk1
        {72, 0, 36, 36, SDL_FLIP_NONE}, //Walk2
        {108, 0, 36 , 36 , SDL_FLIP_NONE}, //Walk3
        {144,0,36,36,SDL_FLIP_NONE}, // Stopping
        {180, 0, 36,36,SDL_FLIP_NONE} //Jump
    };
    player.currentSprite = &sprite_player[0];

    Sprite platformSprites[] = {
        {0, 36, 32, 32, SDL_FLIP_NONE},
        {32, 36, 32, 32, SDL_FLIP_NONE},
    };

    Platform platforms[] = {
        {0, 550, 800, 50, &platformSprites[0]},
        {200, 460, 160, 32, &platformSprites[0]},
        {550, 420, 32,160, &platformSprites[1]},
    };

    int cameraX = 0, cameraY = 0;
    const int cameraOffsetX = 400; // La mitad del ancho de la ventana
    int running = 1;
    SDL_Event event;
    SDL_RenderSetLogicalSize(renderer, 800, 600);

    Uint32 frameStart, frameTime;
    const Uint32 frameDelay = 1000 / 60; // 60 FPS
    int fps = 0;
    Uint32 fpsCounterStart = SDL_GetTicks();
    int frameCount = 0;

    while (running) {
        frameStart = SDL_GetTicks();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        handlePlayerMovement(&player, platforms, sizeof(platforms) / sizeof(platforms[0]), sprite_player);
        // Actualizar la posición de la cámara
        if (player.x > cameraOffsetX) {
            cameraX = player.x - cameraOffsetX;
        }
        SDL_RenderClear(renderer);
        renderSprite(renderer, spriteSheet, player.currentSprite, player.x, player.y, cameraX, cameraY);

        for (size_t i = 0; i < sizeof(platforms) / sizeof(platforms[0]); i++) {
            renderPlatform(renderer, spriteSheet, &platforms[i], cameraX, cameraY);
        }

        // Calcula y muestra el FPS real
        frameCount++;
        if (SDL_GetTicks() - fpsCounterStart >= 1000) {
            fps = frameCount;
            frameCount = 0;
            fpsCounterStart = SDL_GetTicks();
        }

        drawFPS(renderer, font, fps);

        SDL_RenderPresent(renderer);

        frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    SDL_DestroyTexture(spriteSheet);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDLNet_Quit();
    SDL_Quit();

    return 0;
}

void drawFPS(SDL_Renderer* renderer, TTF_Font* font, int fps) {
    char fpsText[10];
    snprintf(fpsText, sizeof(fpsText), "FPS: %d", fps);
    SDL_Color color = {255, 255, 255, 255}; // Blanco
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, fpsText, color);
    SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    SDL_Rect messageRect; // Para renderizar el texto
    messageRect.x = 10;
    messageRect.y = 10;
    messageRect.w = surfaceMessage->w;
    messageRect.h = surfaceMessage->h;

    SDL_RenderCopy(renderer, message, NULL, &messageRect);

    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(message);
}