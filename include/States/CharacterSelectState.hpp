#pragma once

#include "GameState.hpp"
#include <SDL2/SDL.h>
#include <memory>
#include <vector>

class Button;
class Character;

class CharacterSelectState : public GameState {
public:
  CharacterSelectState();
  ~CharacterSelectState() override;

  void handleEvent(const SDL_Event &event, Game *game) override;
  void update(float deltaTime, Game *game) override;
  void render(SDL_Renderer *renderer, Game *game) override;

  void onEnter(Game *game) override;

private:
  SDL_Texture *background;
  std::vector<std::unique_ptr<Button>> buttons;

  Character *player1Character;
  Character *player2Character;

  std::vector<Character *> availableCharacters;
};
