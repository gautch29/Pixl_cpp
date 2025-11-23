#include "UI/HUD.hpp"
#include "Core/Settings.hpp"
#include "Entities/Player.hpp"
#include <string>

HUD::HUD() {}

void HUD::render(SDL_Renderer *renderer, TTF_Font *font, Player *player1,
                 Player *player2, int timeRemaining, int screenWidth,
                 int screenHeight) {

  // Calculate positions based on screen size
  int x1 = screenWidth * 20 / 1920;
  int x2 = screenWidth * 770 / 1920;
  int x3 = screenWidth * 820 / 1920;
  int x4 = screenWidth * 885 / 1920;
  int x5 = screenWidth * 1035 / 1920;
  int x6 = screenWidth * 1100 / 1920;
  int x7 = screenWidth * 1150 / 1920;
  int x8 = screenWidth * 1900 / 1920;

  // Initialize smooth life if first frame
  if (smoothLife1 < 0)
    smoothLife1 = player1->getLife();
  if (smoothLife2 < 0)
    smoothLife2 = player2->getLife();

  // Smoothly interpolate life
  float lerpSpeed = 0.1f;
  smoothLife1 += (player1->getLife() - smoothLife1) * lerpSpeed;
  smoothLife2 += (player2->getLife() - smoothLife2) * lerpSpeed;

  // Render Health Bars
  int barWidth = screenWidth * 0.4;
  int barHeight = 30;
  int margin = 20;

  // Player 1 (Left)
  renderHealthBar(renderer, smoothLife1, 100.0f, margin, margin, barWidth,
                  barHeight, false);
  renderStaminaBar(renderer, player1, margin, margin + barHeight + 5, barWidth,
                   false);

  // Player 2 (Right)
  renderHealthBar(renderer, smoothLife2, 100.0f,
                  screenWidth - margin - barWidth, margin, barWidth, barHeight,
                  true);
  renderStaminaBar(renderer, player2, screenWidth - margin - barWidth,
                   margin + barHeight + 5, barWidth, true);

  // Render Timer
  renderTimer(renderer, font, timeRemaining, screenWidth / 2, margin + 15);
}

void HUD::renderHealthBar(SDL_Renderer *renderer, float life, float maxLife,
                          int x, int y, int width, int height, bool flipped) {
  // Background (Dark Red)
  SDL_Rect bgRect = {x, y, width, height};
  SDL_SetRenderDrawColor(renderer, 50, 0, 0, 255);
  SDL_RenderFillRect(renderer, &bgRect);

  // Foreground (Bright Red)
  float healthPercentage = life / maxLife;
  if (healthPercentage < 0)
    healthPercentage = 0;
  int healthWidth = static_cast<int>(width * healthPercentage);

  SDL_Rect fgRect;
  if (flipped) {
    fgRect = {x + (width - healthWidth), y, healthWidth, height};
  } else {
    fgRect = {x, y, healthWidth, height};
  }

  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderFillRect(renderer, &fgRect);

  // Border (White)
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderDrawRect(renderer, &bgRect);
}

void HUD::renderStaminaBar(SDL_Renderer *renderer, Player *player, int x, int y,
                           int width, bool flipped) {
  float staminaPercent = player->getStamina() / 100.0f;

  SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  int barWidth = static_cast<int>(width * staminaPercent);

  SDL_Rect staminaRect;
  if (flipped) {
    staminaRect = {x + width - barWidth, y, barWidth, 4};
  } else {
    staminaRect = {x, y, barWidth, 4};
  }

  SDL_RenderFillRect(renderer, &staminaRect);
}

void HUD::renderTimer(SDL_Renderer *renderer, TTF_Font *font, int timeRemaining,
                      int centerX, int centerY) {
  std::string timeText = std::to_string(timeRemaining);

  SDL_Color textColor = {0, 0, 0, 255};
  SDL_Surface *surface =
      TTF_RenderText_Blended(font, timeText.c_str(), textColor);
  if (surface) {
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture) {
      SDL_Rect textRect = {centerX - surface->w / 2, centerY - surface->h / 2,
                           surface->w, surface->h};
      SDL_RenderCopy(renderer, texture, nullptr, &textRect);
      SDL_DestroyTexture(texture);
    }
    SDL_FreeSurface(surface);
  }
}
