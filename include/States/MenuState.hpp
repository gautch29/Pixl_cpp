#pragma once

#include "GameState.hpp"
#include <SDL2/SDL.h>
#include <memory>
#include <vector>

class Button;

class MenuState : public GameState {
public:
  MenuState();
  ~MenuState() override;

  void handleEvent(const SDL_Event &event, Game *game) override;
  void update(float deltaTime, Game *game) override;
  void render(SDL_Renderer *renderer, Game *game) override;

  void onEnter(Game *game) override;

private:
  SDL_Texture *background;
  std::vector<std::unique_ptr<Button>> buttons;
};
