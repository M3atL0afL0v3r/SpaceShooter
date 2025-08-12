#include "graphics.h"
#include <cstring>

void drawText(SDL_Renderer* renderer, const char* text, int x, int y, SDL_Color color) {
    TTF_Font* font = TTF_OpenFont("C:/Users/dgaut/OneDrive/Desktop/Projects/C++/SpaceShooter/x64/Debug/Assets/MAGNETOB.ttf", 24);
    if (!font) {
        SDL_Log("Failed to load font");
        return;
    }

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text, 0, color);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FRect dst = { x, y, textSurface->w, textSurface->h };

    SDL_RenderTexture(renderer, textTexture, NULL, &dst);

    SDL_DestroyTexture(textTexture);
    SDL_DestroySurface(textSurface);
    TTF_CloseFont(font);
}
