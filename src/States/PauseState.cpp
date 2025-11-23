#include "States/PauseState.hpp"
#include "Core/Game.hpp"
#include "Core/ResourceManager.hpp"
#include "Input/InputManager.hpp"
#include "States/MenuState.hpp"
#include "UI/Button.hpp"

PauseState::PauseState() {}

PauseState::~PauseState() {}

void PauseState::handleEvent(const SDL_Event &event, Game *game) {
  if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
    game->popState(); // Resume game
    return;
  }

  if (event.type == SDL_MOUSEBUTTONDOWN &&
      event.button.button == SDL_BUTTON_LEFT) {
    int mouseX, mouseY;
    game->getInputManager()->getMousePosition(mouseX, mouseY);

    for (auto &button : buttons) {
      if (button->contains(mouseX, mouseY)) {
        button->onClick();
        break;
      }
    }
  }
}

void PauseState::update(float deltaTime, Game *game) {
  // Pause menu doesn't need updates
}

void PauseState::render(SDL_Renderer *renderer, Game *game) {
  // Draw semi-transparent overlay
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
  SDL_Rect fullScreen = {0, 0, game->getWidth(), game->getHeight()};
  SDL_RenderFillRect(renderer, &fullScreen);

  // Create buttons if not created
  if (buttons.empty()) {
    int screenWidth = game->getWidth();
    int screenHeight = game->getHeight();

    auto resumeButton =
        std::make_unique<Button>(screenWidth * 3 / 8, screenHeight / 2 - 60,
                                 screenWidth / 4, 40, "Resume");
    resumeButton->setCallback([game]() { game->popState(); });
    buttons.push_back(std::move(resumeButton));

    auto menuButton =
        std::make_unique<Button>(screenWidth * 3 / 8, screenHeight / 2 + 20,
                                 screenWidth / 4, 40, "Menu");
    menuButton->setCallback([game]() {
      game->popState();                                 // Pop pause state
      game->changeState(std::make_unique<MenuState>()); // Go to menu
    });
    buttons.push_back(std::move(menuButton));
  }

  // Render buttons
  int mouseX, mouseY;
  game->getInputManager()->getMousePosition(mouseX, mouseY);

  TTF_Font *font =
      game->getResourceManager()->loadFont("assets/Fonts/Alien.ttf", 32);

  for (auto &button : buttons) {
    bool hovered = button->contains(mouseX, mouseY);
    button->render(renderer, font, hovered);
  }
}
