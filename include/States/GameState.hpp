#pragma once

#include <SDL2/SDL.h>

class Game;

class GameState {
public:
  virtual ~GameState() = default;

  virtual void handleEvent(const SDL_Event &event, Game *game) = 0;
  virtual void update(float deltaTime, Game *game) = 0;
  virtual void render(SDL_Renderer *renderer, Game *game) = 0;

  virtual void onEnter(Game *game) {}
  virtual void onExit(Game *game) {}
};
