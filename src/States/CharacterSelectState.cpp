#include "States/CharacterSelectState.hpp"
#include "Core/Game.hpp"
#include "Core/ResourceManager.hpp"
#include "Entities/Character.hpp"
#include "Input/InputManager.hpp"
#include "States/GameplayState.hpp"
#include "UI/Button.hpp"
#include <filesystem>
#include <fstream>
#include <memory>

CharacterSelectState::CharacterSelectState()
    : background(nullptr), player1Character(nullptr),
      player2Character(nullptr) {}

CharacterSelectState::~CharacterSelectState() {
  // No need to delete characters as they are owned by Game
}

void CharacterSelectState::onEnter(Game *game) {
  printf("CharacterSelectState::onEnter\n");
  background = game->getResourceManager()->loadTexture(
      "assets/Backgrounds/CharSelect_BG.png");

  // Get available characters from Game
  const auto &gameCharacters = game->getCharacters();
  availableCharacters.clear();
  for (const auto &character : gameCharacters) {
    availableCharacters.push_back(character.get());
  }

  // Set default selections
  if (!availableCharacters.empty()) {
    player1Character = availableCharacters[0];
    player2Character = availableCharacters[0];
  }

  int screenWidth = game->getWidth();
  int screenHeight = game->getHeight();
  int profileSize = 120;
  int spacing = 30;

  // Back button
  auto backButton = std::make_unique<Button>(50, 50, 150, 50, "BACK");
  backButton->setCallback([game]() { game->popState(); });
  backButton->setColors({0, 0, 0, 150}, {255, 50, 50, 50},
                        {255, 255, 255, 255});
  buttons.push_back(std::move(backButton));

  // Start game button
  auto startButton = std::make_unique<Button>(
      screenWidth / 2 - 150, screenHeight - 120, 300, 80, "FIGHT!");
  startButton->setCallback([this, game]() {
    printf("Fight button clicked\n");
    if (player1Character && player2Character) {
      printf("Starting GameplayState with P1: %s, P2: %s\n",
             player1Character->name.c_str(), player2Character->name.c_str());
      game->changeState(
          std::make_unique<GameplayState>(player1Character, player2Character));
    } else {
      printf("Cannot start: P1 or P2 character is null\n");
    }
  });
  startButton->setColors({0, 0, 0, 150}, {255, 0, 0, 100},
                         {255, 255, 255, 255});
  buttons.push_back(std::move(startButton));

  // Character selection buttons for player 1
  int p1StartX = screenWidth / 4 -
                 (availableCharacters.size() * (profileSize + spacing)) / 2;
  int p1StartY = screenHeight / 2;

  for (size_t i = 0; i < availableCharacters.size(); i++) {
    int x = p1StartX + i * (profileSize + spacing);
    int y = p1StartY;

    auto button = std::make_unique<Button>(x, y, profileSize, profileSize, "");
    Character *character = availableCharacters[i];
    button->setCallback([this, character]() { player1Character = character; });
    button->setColors({0, 0, 0, 0}, {0, 255, 255, 50},
                      {0, 0, 0, 0}); // Transparent normal, cyan hover
    buttons.push_back(std::move(button));
  }

  // Character selection buttons for player 2
  int p2StartX = screenWidth * 3 / 4 -
                 (availableCharacters.size() * (profileSize + spacing)) / 2;
  int p2StartY = screenHeight / 2;

  for (size_t i = 0; i < availableCharacters.size(); i++) {
    int x = p2StartX + i * (profileSize + spacing);
    int y = p2StartY;

    auto button = std::make_unique<Button>(x, y, profileSize, profileSize, "");
    Character *character = availableCharacters[i];
    button->setCallback([this, character]() { player2Character = character; });
    button->setColors({0, 0, 0, 0}, {255, 0, 255, 50},
                      {0, 0, 0, 0}); // Transparent normal, magenta hover
    buttons.push_back(std::move(button));
  }
}

void CharacterSelectState::handleEvent(const SDL_Event &event, Game *game) {
  if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
    game->popState();
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

void CharacterSelectState::update(float deltaTime, Game *game) {}

void CharacterSelectState::render(SDL_Renderer *renderer, Game *game) {
  if (background) {
    SDL_RenderCopy(renderer, background, nullptr, nullptr);
  }

  TTF_Font *font =
      game->getResourceManager()->loadFont("assets/Fonts/Alien.ttf", 50);
  SDL_Color white = {255, 255, 255, 255};

  // Render "Player 1" and "Player 2" titles
  SDL_Color cyan = {0, 255, 255, 255};
  SDL_Color magenta = {255, 0, 255, 255};

  // Grid Layout Settings
  int profileSize = 150; // Increased size to fill slots
  int cols = 2;

  // P1 Grid Start - Adjusted based on screenshot
  // Slots appear to be roughly at 13% and 31% width
  int p1StartX = game->getWidth() * 0.135;
  int p1StartY = game->getHeight() * 0.38;
  int p1GapX = game->getWidth() * 0.185; // Distance between columns
  int p1GapY = game->getHeight() * 0.27; // Distance between rows

  // P2 Grid Start - Symmetric to P1
  // Slots appear to be roughly at 58% and 76% width
  int p2StartX = game->getWidth() * 0.585;
  int p2StartY = game->getHeight() * 0.38;
  int p2GapX = game->getWidth() * 0.185;
  int p2GapY = game->getHeight() * 0.27;

  // P1 Header
  SDL_Surface *p1Surface = TTF_RenderText_Blended(font, "PLAYER 1", cyan);
  if (p1Surface) {
    SDL_Texture *p1Texture = SDL_CreateTextureFromSurface(renderer, p1Surface);
    SDL_Rect p1Rect = {game->getWidth() / 4 - p1Surface->w / 2,
                       static_cast<int>(game->getHeight() * 0.20), p1Surface->w,
                       p1Surface->h};
    SDL_RenderCopy(renderer, p1Texture, nullptr, &p1Rect);
    SDL_DestroyTexture(p1Texture);
    SDL_FreeSurface(p1Surface);
  }

  // P2 Header
  SDL_Surface *p2Surface = TTF_RenderText_Blended(font, "PLAYER 2", magenta);
  if (p2Surface) {
    SDL_Texture *p2Texture = SDL_CreateTextureFromSurface(renderer, p2Surface);
    SDL_Rect p2Rect = {game->getWidth() * 3 / 4 - p2Surface->w / 2,
                       static_cast<int>(game->getHeight() * 0.20), p2Surface->w,
                       p2Surface->h};
    SDL_RenderCopy(renderer, p2Texture, nullptr, &p2Rect);
    SDL_DestroyTexture(p2Texture);
    SDL_FreeSurface(p2Surface);
  }

  // Render buttons first (so portraits are on top)
  int mouseX, mouseY;
  game->getInputManager()->getMousePosition(mouseX, mouseY);

  TTF_Font *buttonFont =
      game->getResourceManager()->loadFont("assets/Fonts/Alien.ttf", 32);
  for (auto &button : buttons) {
    bool hovered = button->contains(mouseX, mouseY);
    button->render(renderer, buttonFont, hovered);
  }

  // Render character portraits
  for (size_t i = 0; i < availableCharacters.size(); i++) {
    Character *character = availableCharacters[i];
    if (character->profilePicture) {
      int row = i / cols;
      int col = i % cols;

      // Player 1 portraits
      int p1X = p1StartX + col * p1GapX;
      int p1Y = p1StartY + row * p1GapY;
      SDL_Rect p1Rect = {p1X, p1Y, profileSize, profileSize};
      SDL_RenderCopy(renderer, character->profilePicture, nullptr, &p1Rect);

      // Highlight selected P1 character
      if (player1Character == character) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255); // Cyan border
        SDL_Rect border = {p1Rect.x - 4, p1Rect.y - 4, p1Rect.w + 8,
                           p1Rect.h + 8};
        SDL_RenderDrawRect(renderer, &border);
        SDL_Rect border2 = {p1Rect.x - 3, p1Rect.y - 3, p1Rect.w + 6,
                            p1Rect.h + 6};
        SDL_RenderDrawRect(renderer, &border2);
      }

      // Player 2 portraits
      int p2X = p2StartX + col * p2GapX;
      int p2Y = p2StartY + row * p2GapY;
      SDL_Rect p2Rect = {p2X, p2Y, profileSize, profileSize};
      SDL_RenderCopy(renderer, character->profilePicture, nullptr, &p2Rect);

      // Highlight selected P2 character
      if (player2Character == character) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255); // Magenta border
        SDL_Rect border = {p2Rect.x - 4, p2Rect.y - 4, p2Rect.w + 8,
                           p2Rect.h + 8};
        SDL_RenderDrawRect(renderer, &border);
        SDL_Rect border2 = {p2Rect.x - 3, p2Rect.y - 3, p2Rect.w + 6,
                            p2Rect.h + 6};
        SDL_RenderDrawRect(renderer, &border2);
      }
    }
  }
}
