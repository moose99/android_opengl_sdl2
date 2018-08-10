#pragma once

#include <string>
#include <iostream>

#include "SDL.h"

void logSDLError(std::ostream &os, const std::string &msg);
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, SDL_Rect dst, SDL_Rect *clip = nullptr);
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, SDL_Rect *clip = nullptr);

