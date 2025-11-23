#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class Player;
struct Settings;

class HUD {
public:
  HUD();

  void render(SDL_Renderer *renderer, TTF_Font *font, Player *player1,
              Player *player2, int timeRemaining, int screenWidth,
              int screenHeight);

private:
  void renderHealthBar(SDL_Renderer *renderer, float life, float maxLife, int x,
                       int y, int width, int height, bool flipped);
  void renderStaminaBar(SDL_Renderer *renderer, Player *player, int x, int y,
                        int width, bool flipped);
  void renderTimer(SDL_Renderer *renderer, TTF_Font *font, int timeRemaining,
                   int centerX, int centerY);

  float smoothLife1 = -1.0f;
  float smoothLife2 = -1.0f;
};
