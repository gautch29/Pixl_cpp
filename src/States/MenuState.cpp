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
  printf("MenuState::onEnter\n");
  // Load background
  background =
      game->getResourceManager()->loadTexture("assets/Backgrounds/Menu_BG.png");

  if (!background)
    printf("Failed to load Menu background\n");

  int screenWidth = game->getWidth();
  int screenHeight = game->getHeight();

  // Create buttons
  TTF_Font *font =
      game->getResourceManager()->loadFont("assets/Fonts/Alien.ttf", 50);

  if (!font)
    printf("Failed to load Menu font\n");

  int buttonWidth = 400;
  int buttonHeight = 60;
  int startY = screenHeight / 2;
  int spacing = 20;

  // Create menu buttons
  auto playButton =
      std::make_unique<Button>((screenWidth - buttonWidth) / 2, startY,
                               buttonWidth, buttonHeight, "PLAY");
  playButton->setCallback(
      [game]() { game->pushState(std::make_unique<CharacterSelectState>()); });
  playButton->setColors({0, 0, 0, 150}, {0, 255, 255, 50},
                        {255, 255, 255, 255});
  buttons.push_back(std::move(playButton));

  auto settingsButton = std::make_unique<Button>(
      (screenWidth - buttonWidth) / 2, startY + buttonHeight + spacing,
      buttonWidth, buttonHeight, "SETTINGS");
  settingsButton->setCallback(
      [game]() { game->pushState(std::make_unique<SettingsState>()); });
  settingsButton->setColors({0, 0, 0, 150}, {255, 0, 255, 50},
                            {255, 255, 255, 255});
  buttons.push_back(std::move(settingsButton));

  auto exitButton = std::make_unique<Button>(
      (screenWidth - buttonWidth) / 2, startY + (buttonHeight + spacing) * 2,
      buttonWidth, buttonHeight, "EXIT");
  exitButton->setCallback([game]() { game->quit(); });
  exitButton->setColors({0, 0, 0, 150}, {255, 50, 50, 50},
                        {255, 255, 255, 255});
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

  if (!font)
    printf("Failed to load Menu font in render\n");

  for (auto &button : buttons) {
    bool hovered = button->contains(mouseX, mouseY);
    button->render(renderer, font, hovered);
  }
}
