#pragma once

#include "GameState.hpp"
#include <SDL2/SDL.h>
#include <memory>
#include <vector>

class Button;

class PauseState : public GameState {
public:
  PauseState();
  ~PauseState() override;

  void handleEvent(const SDL_Event &event, Game *game) override;
  void update(float deltaTime, Game *game) override;
  void render(SDL_Renderer *renderer, Game *game) override;

private:
  std::vector<std::unique_ptr<Button>> buttons;
};
