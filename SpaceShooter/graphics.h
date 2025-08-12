#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

void drawText(SDL_Renderer* renderer, const char* text, int x, int y, SDL_Color color);

#endif
