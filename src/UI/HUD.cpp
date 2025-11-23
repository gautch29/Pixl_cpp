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

  int y1 = screenHeight * 20 / 1080;
  int y2 = screenHeight * 40 / 1080;
  int y3 = screenHeight * 100 / 1080;
  int y4 = screenHeight * 120 / 1080;

  // Render health bars
  renderHealthBar(renderer, player1, x1, y1, x2 - x1, y3 - y1, false);
  renderHealthBar(renderer, player2, x7, y2, x8 - x7, y4 - y2, true);

  // Render stamina bars
  renderStaminaBar(renderer, player1, x1, y4, screenWidth * 760 / 1920 - x1,
                   false);
  renderStaminaBar(renderer, player2, x7, y1, x8 - screenWidth * 1160 / 1920,
                   true);

  // Render timer background
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_Point timerPoints[] = {
      {x4, y1}, {x5 + (x6 - x5), y1}, {x5, y4}, {x3 + (x4 - x3), y4}};
  SDL_RenderDrawLines(renderer, timerPoints, 4);

  // Render timer
  renderTimer(renderer, font, timeRemaining, screenWidth / 2, (y1 + y4) / 2);
}

void HUD::renderHealthBar(SDL_Renderer *renderer, Player *player, int x, int y,
                          int width, int height, bool flipped) {
  float lifePercent = player->getLife() / 100.0f;
  SDL_Color color = (player->getPosition().x < 960) ? SDL_Color{255, 0, 0, 255}
                                                    : SDL_Color{0, 0, 255, 255};

  // Fill health bar
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
  int barWidth = static_cast<int>(width * lifePercent);
  SDL_Rect healthRect;

  if (flipped) {
    healthRect = {x + width - barWidth, y, barWidth, height};
  } else {
    healthRect = {x, y, barWidth, height};
  }

  SDL_RenderFillRect(renderer, &healthRect);

  // Draw border
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_Rect borderRect = {x, y, width, height};
  SDL_RenderDrawRect(renderer, &borderRect);
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
