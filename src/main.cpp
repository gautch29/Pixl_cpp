#include "Core/Game.hpp"
#include "States/MenuState.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  Game game;

  if (!game.initialize()) {
    std::cerr << "Failed to initialize game!" << std::endl;
    return 1;
  }

  // Start with menu state
  game.pushState(std::make_unique<MenuState>());

  game.run();
  game.cleanup();

  return 0;
}
