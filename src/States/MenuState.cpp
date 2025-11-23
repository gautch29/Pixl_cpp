#include "States/MenuState.hpp"
#include "Core/Game.hpp"
#include "Core/ResourceManager.hpp"
#include "Input/InputManager.hpp"
#include "States/CharacterSelectState.hpp"
#include "States/SettingsState.hpp"
#include "UI/Button.hpp"
#include <memory>

MenuState::MenuState() : background(nullptr) {}

MenuState::~MenuState() {}

void MenuState::onEnter(Game *game) {
  // Load background
  background = game->getResourceManager()->loadTexture(
      "assets/Backgrounds/Background 2.jpg");

  int screenWidth = game->getWidth();
  int screenHeight = game->getHeight();

  // Create menu buttons
  auto playButton =
      std::make_unique<Button>(0, 0, screenWidth / 4, screenHeight / 4, "Play");
  playButton->setCallback(
      [game]() { game->pushState(std::make_unique<CharacterSelectState>()); });
  buttons.push_back(std::move(playButton));

  auto settingsButton = std::make_unique<Button>(
      0, screenHeight / 4, screenWidth / 4, screenHeight / 4, "Settings");
  settingsButton->setCallback(
      [game]() { game->pushState(std::make_unique<SettingsState>()); });
  buttons.push_back(std::move(settingsButton));

  auto charactersButton = std::make_unique<Button>(
      0, screenHeight / 2, screenWidth / 4, screenHeight / 4, "Characters");
  charactersButton->setCallback(
      [game]() { game->pushState(std::make_unique<CharacterSelectState>()); });
  buttons.push_back(std::move(charactersButton));

  auto exitButton = std::make_unique<Button>(
      0, screenHeight * 3 / 4, screenWidth / 4, screenHeight / 4, "Exit");
  exitButton->setCallback([game]() { game->quit(); });
  buttons.push_back(std::move(exitButton));
}

void MenuState::handleEvent(const SDL_Event &event, Game *game) {
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

void MenuState::update(float deltaTime, Game *game) {
  // Menu doesn't need updates
}

void MenuState::render(SDL_Renderer *renderer, Game *game) {
  // Render background
  if (background) {
    SDL_RenderCopy(renderer, background, nullptr, nullptr);
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
