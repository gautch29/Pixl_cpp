#pragma once

#include "GameState.hpp"
#include <SDL2/SDL.h>
#include <memory>

class Player;
class Character;
struct Settings;
class HUD;

class GameplayState : public GameState {
public:
  GameplayState(Character *char1, Character *char2);
  ~GameplayState() override;

  void handleEvent(const SDL_Event &event, Game *game) override;
  void update(float deltaTime, Game *game) override;
  void render(SDL_Renderer *renderer, Game *game) override;

  void onEnter(Game *game) override;
  void onExit(Game *game) override;

private:
  void checkCollisions();
  void checkWinCondition(Game *game);

  SDL_Texture *background;
  std::unique_ptr<Player> player1;
  std::unique_ptr<Player> player2;
  std::unique_ptr<HUD> hud;

  Character *character1;
  Character *character2;
  Settings *settings;

  Uint32 startTime;
  Uint32 pausedTime;
  Uint32 lastUpdateTime;
  bool lifeInitialized;
  bool finished;

  float shakeTimer;
  float shakeMagnitude;
};
