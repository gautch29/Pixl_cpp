#pragma once

#include <SDL2/SDL.h>
#include <map>
#include <string>

enum class PlayerControl { UP, LEFT, DOWN, RIGHT, ATTACK, SECONDARY };

struct PlayerInput {
  bool controls[6] = {false};
  SDL_Keycode keys[6];
};

class InputManager {
public:
  InputManager();

  void handleEvent(const SDL_Event &event);
  void update();

  bool isKeyPressed(SDL_Keycode key) const;
  bool isMouseButtonPressed(Uint8 button) const;

  void getMousePosition(int &x, int &y) const;

  PlayerInput &getPlayer1Input() { return player1; }
  PlayerInput &getPlayer2Input() { return player2; }

  bool loadInputConfig(const std::string &filename);

private:
  std::map<SDL_Keycode, bool> keyStates;
  std::map<Uint8, bool> mouseButtonStates;
  int mouseX, mouseY;

  PlayerInput player1;
  PlayerInput player2;

  void updatePlayerControl(PlayerInput &player, SDL_Keycode key, bool pressed);
};
