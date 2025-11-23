#include "Input/InputManager.hpp"
#include <fstream>
#include <sstream>

InputManager::InputManager() : mouseX(0), mouseY(0) {
  // Default key bindings (will be overridden by config file)
  player1.keys[0] = SDLK_w;      // UP
  player1.keys[1] = SDLK_a;      // LEFT
  player1.keys[2] = SDLK_s;      // DOWN
  player1.keys[3] = SDLK_d;      // RIGHT
  player1.keys[4] = SDLK_SPACE;  // ATTACK
  player1.keys[5] = SDLK_LSHIFT; // SECONDARY

  player2.keys[0] = SDLK_UP;     // UP
  player2.keys[1] = SDLK_LEFT;   // LEFT
  player2.keys[2] = SDLK_DOWN;   // DOWN
  player2.keys[3] = SDLK_RIGHT;  // RIGHT
  player2.keys[4] = SDLK_RCTRL;  // ATTACK
  player2.keys[5] = SDLK_RSHIFT; // SECONDARY
}

void InputManager::handleEvent(const SDL_Event &event) {
  if (event.type == SDL_KEYDOWN) {
    keyStates[event.key.keysym.sym] = true;
    updatePlayerControl(player1, event.key.keysym.sym, true);
    updatePlayerControl(player2, event.key.keysym.sym, true);
  } else if (event.type == SDL_KEYUP) {
    keyStates[event.key.keysym.sym] = false;
    updatePlayerControl(player1, event.key.keysym.sym, false);
    updatePlayerControl(player2, event.key.keysym.sym, false);
  } else if (event.type == SDL_MOUSEBUTTONDOWN) {
    mouseButtonStates[event.button.button] = true;
  } else if (event.type == SDL_MOUSEBUTTONUP) {
    mouseButtonStates[event.button.button] = false;
  } else if (event.type == SDL_MOUSEMOTION) {
    mouseX = event.motion.x;
    mouseY = event.motion.y;
  }
}

void InputManager::update() {
  // Update mouse position
  SDL_GetMouseState(&mouseX, &mouseY);
}

bool InputManager::isKeyPressed(SDL_Keycode key) const {
  auto it = keyStates.find(key);
  return it != keyStates.end() && it->second;
}

bool InputManager::isMouseButtonPressed(Uint8 button) const {
  auto it = mouseButtonStates.find(button);
  return it != mouseButtonStates.end() && it->second;
}

void InputManager::getMousePosition(int &x, int &y) const {
  x = mouseX;
  y = mouseY;
}

void InputManager::updatePlayerControl(PlayerInput &player, SDL_Keycode key,
                                       bool pressed) {
  for (int i = 0; i < 6; i++) {
    if (player.keys[i] == key) {
      player.controls[i] = pressed;
      break;
    }
  }
}

bool InputManager::loadInputConfig(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    return false;
  }

  const char *controlNames[] = {"up",    "left",   "down",
                                "right", "attack", "secondary"};

  std::string line;
  while (std::getline(file, line)) {
    if (line.empty())
      continue;

    size_t equalPos = line.find('=');
    if (equalPos == std::string::npos)
      continue;

    std::string key = line.substr(0, line.find(' '));
    std::string value =
        line.substr(equalPos + 2); // Get the value string (e.g., "SPACE")

    // Remove any trailing whitespace/newline from value
    while (!value.empty() && (value.back() == '\r' || value.back() == '\n' ||
                              value.back() == ' ')) {
      value.pop_back();
    }

    // Convert string to SDL_Keycode
    SDL_Keycode keycode = SDL_GetKeyFromName(value.c_str());
    if (keycode == SDLK_UNKNOWN) {
      // Fallback for single characters if GetKeyFromName fails or for simple
      // chars
      if (value.length() == 1) {
        keycode = static_cast<SDL_Keycode>(value[0]);
      }
    }

    for (int i = 0; i < 6; i++) {
      std::string player1Key = "player1_" + std::string(controlNames[i]);
      std::string player2Key = "player2_" + std::string(controlNames[i]);

      if (key == player1Key) {
        player1.keys[i] = keycode;
      } else if (key == player2Key) {
        player2.keys[i] = keycode;
      }
    }
  }

  file.close();
  return true;
}
